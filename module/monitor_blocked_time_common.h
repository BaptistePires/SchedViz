#ifndef MONITOR_BLOCKED_TIME_COMMON
#define MONITOR_BLOCKED_TIME_COMMON

#define BUFFER_SIZE 20000
#define TASK_DEBUGFS_DIR "/sys/kernel/debug/sched/tasks"



#ifndef __KERNEL__
#include <stdint.h>
#define ktime_t uint64_t
#define u64 uint64_t
#endif

struct log_entry {
    u64 wakeup_date;
    u64 duration;
};

#endif // MONITOR_BLOCKED_TIME_COMMON