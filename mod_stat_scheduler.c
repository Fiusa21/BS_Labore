#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Module to run work_handler recurrently");

#define DEFAULT_DELAY_MS 1000

static unsigned long delay_ms = DEFAULT_DELAY_MS;

module_param(delay_ms, ulong, 0644);
MODULE_PARM_DESC(delay_ms, "Delay in milliseconds for work_handler execution");

static struct workqueue_struct *wq = NULL;
static DECLARE_DELAYED_WORK(dwork, work_handler);

static void work_handler(struct work_struct *w) {
    static int times = 0;
    int i;
    struct task_struct *task = current;
    struct latency_record *latency_record = &task->latency_record;

    printk(KERN_DEBUG "work_handler runs w:%pX the %d. time\n", w, times++);

    if (latency_record) {
        printk(KERN_DEBUG "Latency records for current process:\n");
        for (i = 0; i < latency_record->count; ++i) {
            printk(KERN_DEBUG "Latency %d: %llu\n", i, latency_record->latency[i]);
        }
    } else {
        printk(KERN_DEBUG "No latency records available for current process\n");
    }

    queue_delayed_work(wq, &dwork, msecs_to_jiffies(delay_ms));
}

static int __init init_module(void) {
    printk(KERN_DEBUG "Hello from module\n");
    wq = alloc_workqueue("test", WQ_UNBOUND, 1);
    if (!wq) {
        pr_err("Failed to allocate workqueue\n");
        return -ENOMEM;
    }
    queue_delayed_work(wq, &dwork, msecs_to_jiffies(delay_ms));
    return 0;
}

static void __exit cleanup_module(void) {
    cancel_delayed_work_sync(&dwork);
    destroy_workqueue(wq);
}

module_init(init_module);
module_exit(cleanup_module);
