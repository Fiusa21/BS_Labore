#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Name");
MODULE_DESCRIPTION("Module to schedule work_handler with specified delay and output latency records with backtraces");

#define DEFAULT_DELAY_MS 1000
#define LATENCY_RECORD_COUNT 32

/*
 * Function definitions
 */
static void work_handler(struct work_struct *w);
static void output_latency_records(void);

/*
 * Data declarations
 */
static struct workqueue_struct *wq = NULL;
static DECLARE_DELAYED_WORK(dwork, work_handler);
static unsigned long delay = msecs_to_jiffies(DEFAULT_DELAY_MS);

static int delay_ms = DEFAULT_DELAY_MS;
module_param(delay_ms, int, 0644); // Module parameter for delay in milliseconds
MODULE_PARM_DESC(delay_ms, "Delay in milliseconds for work_handler to run again");

static void work_handler(struct work_struct *w) {
    static int times = 0;
    printk(KERN_DEBUG "work_handler runs w:%pX the %d. time\n", w, times++); // rather pr_debug()
    output_latency_records();
    queue_delayed_work(wq, &dwork, delay);
}

static void output_latency_records(void) {
    int i;
    struct task_struct *task = current;
    struct sched_info *si = &task->sched_info;
    u64 *latency_record = si->latency_record;

    pr_info("Latency Records with Backtraces:\n");
    for (i = 0; i < LATENCY_RECORD_COUNT; ++i) {
        unsigned long backtrace_value = si->latency_record_backtrace[i][0];
        pr_info("latency_record[%d]: %llu, backtrace[0]: %pX\n", i, latency_record[i], (void *)backtrace_value);
    }
}

static int __init mod_stat_scheduler_init(void) {
    printk(KERN_DEBUG "Initializing mod_stat_scheduler\n");
    delay = msecs_to_jiffies(delay_ms);
    wq = alloc_workqueue("test", WQ_UNBOUND, 1);
    if (!wq) {
        pr_err("Cannot allocate workqueue\n");
        return -ENOMEM;
    }
    queue_delayed_work(wq, &dwork, delay);
    return 0;
}

static void __exit mod_stat_scheduler_exit(void) {
    cancel_delayed_work_sync(&dwork);
    destroy_workqueue(wq);
    printk(KERN_DEBUG "Exiting mod_stat_scheduler\n");
}

module_init(mod_stat_scheduler_init);
module_exit(mod_stat_scheduler_exit);
