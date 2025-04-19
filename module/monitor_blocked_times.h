#ifndef MONITOR_BLOCKED_TIME
#define MONITOR_BLOCKED_TIME

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/kprobes.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/spinlock_types.h>
#include <linux/kref.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>

#include "monitor_blocked_time_common.h"

#define TASK_DATA_SIZE sizeof(task_data) + BUFFER_SIZE * sizeof(struct log_entry)
#define S64_AS_STR_SZ 19
#define MONITORED_TASK 1 << 0
#define KERNEL_THREAD  1 << 1
#define OTHER_TASK     1 << 2

#define ADD_TASK_DEBUGFS(pid) add_task((pid), 1)
#define ADD_TASK_NOT_MONITORED(pid) add_task((pid), 0)



struct task_data {
    struct list_head list;
    pid_t pid;
    size_t pos;
    struct dentry *read_file;
    spinlock_t lock;
    struct kref kref;
    uint8_t flag;
    char comm[TASK_COMM_LEN];
    struct log_entry data[0];
};


static int add_task(pid_t pid, int app_related);
static int add_pid_op(void *data, u64 value);
static int remove_task(pid_t task);
static void task_release(struct kref *kref);
static ssize_t read_task_file(struct file *f, char __user *buf, size_t count,
		loff_t *ppos);
void update_time_blocked(pid_t pid, u64 wakeup_date, u64 duration);

#endif // MONITOR_BLOCKED_TIME