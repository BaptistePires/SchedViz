#include "monitor_blocked_times.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Baptiste Pires");
MODULE_DESCRIPTION("Simple module to monitor the time threads spend in a blocked state");

static const char *dir_name = "sched";
static const char *dir_name_monitored_tasks = "user";
static const char *dir_name_kernel_tasks = "kernel";
static const char *dir_name_other_tasks = "other";
static const char *add_pid_fname = "add";

static struct dentry *sched_dir;
static struct dentry *add_file;
static struct dentry *monitored_read_dir;
static struct dentry *kernel_read_dir;
static struct dentry *other_read_dir;

DEFINE_SIMPLE_ATTRIBUTE(add_fops, NULL, add_pid_op, "%llu\n");

static LIST_HEAD(tasks_data);

static DEFINE_SPINLOCK(tasks_lock);
static pid_t tasks[10000] = {0};
static size_t t_count = 0;

static const struct file_operations fops_task = {
	.owner = THIS_MODULE,
	.read = read_task_file,
};

extern void (*update_blocked_time_fn)(pid_t pid, u64 wakeup_date, u64 duration);
static int tmp;

static ssize_t read_task_file(struct file *f, char __user *buf, size_t count,
		loff_t *ppos)
{
	ssize_t ret;
	struct task_data *tsk = (struct task_data *) f->f_inode->i_private;
	
	spin_lock(&tsk->lock);
	ret = simple_read_from_buffer(buf, count, ppos, tsk->data, count);
	spin_unlock(&tsk->lock);
	return ret;
}

static int __init monitor_init(void)
{

    // Creating main directory
	sched_dir = debugfs_create_dir(dir_name, 0);
	if (!sched_dir) {
		pr_err("Can't create entry %s in debugfs...\n", dir_name);
		goto err;
	}

	monitored_read_dir = debugfs_create_dir(dir_name_monitored_tasks, sched_dir);
	if (!monitored_read_dir) {
		pr_err("Can't create entry /sys/kernel/debug/sched/%s.\n", dir_name_monitored_tasks);
		goto err;
	}	
	
	kernel_read_dir = debugfs_create_dir(dir_name_kernel_tasks, sched_dir);
	if (!kernel_read_dir) {
		pr_err("Can't create entry /sys/kernel/debug/sched/%s.\n",
				dir_name_kernel_tasks);
		goto err;
	}

	other_read_dir = debugfs_create_dir(dir_name_other_tasks, sched_dir);
	if (!other_read_dir) {
		pr_err("Can't create entry /sys/kernel/debug/sched/%s.\n", dir_name_other_tasks);
		goto err;
	}


	add_file = debugfs_create_file(add_pid_fname, 0700, sched_dir, NULL,
				&add_fops);
	if (!add_file) {
		pr_err("Can't create file /sys/kernel/debug/%s.\n",
			add_pid_fname);
		goto err;
	}

	update_blocked_time_fn = update_time_blocked;

	// spin_lock_init(&tasks_lock);
	pr_info("Module monitor_block_times loaded.\n");

	return 0;

err:
	pr_err("Module %s not loaded.\n", THIS_MODULE->name);
	debugfs_remove_recursive(sched_dir);

	tmp = -1;
	return -1;
}

module_init(monitor_init);


static void __exit monitor_exit(void)
{
	struct task_data *pos, *next;
	update_blocked_time_fn = NULL;

	list_for_each_entry_safe(pos, next, &tasks_data, list) {
		// pr_info("%d\n", pos->pid);
		remove_task(pos->pid);
	}

	debugfs_remove_recursive(sched_dir);
}

module_exit(monitor_exit);

void update_time_blocked(pid_t pid, u64 wakeup_date, u64 duration)
{
	struct task_data *cursor;
	
	int found = 0;

#ifdef __DEBUG_BT
	struct task_struct *__debug_ts;
	__debug_ts = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
	if (strncmp(__debug_ts->comm, "sleepy", 6) == 0)
		pr_info("%d - blocked_time : %llu\n", pid, blocked_time);
#endif

    // Here we can store somewhere the value sent by the task
retry:
	list_for_each_entry(cursor, &tasks_data, list) {
		kref_get(&cursor->kref);
		if (cursor->pid == pid) {
			spin_lock(&cursor->lock);
			if (cursor->pos < BUFFER_SIZE){
				cursor->data[cursor->pos].duration = duration;
				cursor->data[cursor->pos++].wakeup_date = wakeup_date;
			}
			spin_unlock(&cursor->lock);
			found = 1;
		}
		kref_put(&cursor->kref, task_release);
	}

	if (!found) {
		ADD_TASK_NOT_MONITORED(pid);
		goto retry;
	}
}

static int add_task(pid_t pid, int app_related)
{
	struct task_struct *target;
	struct task_data *task_data;
	unsigned long flags;
	char entry_name[25];
	size_t i;

	// if task is already being monitored, remove it
	// if (remove_task(pid))
	// 	return 0;
	spin_lock_irq(&tasks_lock);
	for(i = 0; i < t_count; i++) {
		if (pid == tasks[i]) {
			spin_unlock_irq(&tasks_lock);
			return 0;
		}
	}
	tasks[t_count++] = pid;
	// pr_info("adding %d, pos %i\n", pid, t_count);
	spin_unlock_irq(&tasks_lock);

	target = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
	if (!target)  {
		pr_warn("Task with pid %d doesn't exist\n", pid);
		return -1;
	}

	// Here tasks exists
	task_data = vmalloc(TASK_DATA_SIZE);
	if (!task_data) {
		pr_warn("Error allocating task_data structure for pid %d\n",
			pid);
		return -1;
	}

	memset(task_data, 0, TASK_DATA_SIZE);
	task_data->pid = pid;
	task_data->pos = 0;
	spin_lock_init(&task_data->lock);
	kref_init(&task_data->kref);

	list_add(&task_data->list, &tasks_data);
	// target->update_state = update_time_blocked;

	snprintf(entry_name, 11, "%d", pid);
	// Setting up debugfs
	if (target->is_monitored || app_related) {
		task_data->read_file = debugfs_create_file(entry_name, 0700,
						monitored_read_dir
					, task_data, &fops_task);
	} else if (target->flags & PF_KTHREAD) {
		task_data->read_file = debugfs_create_file(entry_name, 0700,
								kernel_read_dir, task_data, &fops_task);
	} else {
		task_data->read_file = debugfs_create_file(entry_name, 0700,
								other_read_dir, task_data, &fops_task);
	}

	return 0;

}

static int add_pid_op(void *data, u64 value)
{
	struct task_data *cursor;
	struct task_struct *target;
	char entry_name[11];
	pid_t pid= (pid_t) value;

	// pr_info("add : %d\n", pid);

	target = get_pid_task(find_get_pid(pid), PIDTYPE_PID);

	if (target)	target->is_monitored = 1;
	// pr_info("is_mo : %d\n", target->is_monitored);
	list_for_each_entry(cursor, &tasks_data, list) {
		if (cursor->pid == pid) {
			debugfs_remove(cursor->read_file);
			snprintf(entry_name, 11, "%d", pid);
			cursor->read_file = debugfs_create_file(entry_name, 0700,
				monitored_read_dir
			, cursor, &fops_task);

			return 0;
		}
	}
	return ADD_TASK_DEBUGFS(pid);
}

int remove_task(pid_t pid)
{
	struct task_struct *target;
	struct task_data *cursor, *next;

	target = get_pid_task(find_get_pid(pid), PIDTYPE_PID);

	// Task could be dead
	if (target) {
		target->is_monitored = 0;
		target->update_state = NULL;
	}


	list_for_each_entry_safe(cursor, next, &tasks_data, list) {
		if (cursor->pid == pid) {
			// list_del(&cursor->list);
			// kfree(cursor);
			kref_put(&cursor->kref, task_release);
		}
	}

	return 0;
}


static void task_release(struct kref *kref)
{
	struct task_data *data = container_of(kref, struct task_data, kref);

	list_del(&data->list);
	vfree(data);
}
