#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/sched.h> // for current

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Kernel module for scheduling periodic statistics output");
MODULE_VERSION("0.1");

struct latency_record {
    unsigned int count;
    unsigned long backtrace[10]; // assuming 10 entries for backtrace
};

static struct workqueue_struct *stat_workqueue;
static struct delayed_work stat_work;
static unsigned int delay_ms = 1000; // default delay in milliseconds

static void work_handler(struct work_struct *work) {
    struct latency_record *record;
    record = kmalloc(sizeof(struct latency_record), GFP_KERNEL);
    if (!record) {
        printk(KERN_ERR "Failed to allocate memory for latency record\n");
        return;
    }

    // Populate your statistics data here
    // For example, you can retrieve latency records from current process

    // Example output
    printk(KERN_INFO "Latency Records:\n");
    printk(KERN_INFO "  Count: %u\n", record->count);
    printk(KERN_INFO "  Backtrace[0]: %pX\n", (void *)record->backtrace[0]);

    kfree(record);

    // Reschedule the work
    queue_delayed_work(stat_workqueue, &stat_work, msecs_to_jiffies(delay_ms));
}

static int __init mod_stat_scheduler_init(void) {
    printk(KERN_INFO "mod_stat_scheduler module loaded\n");

    stat_workqueue = create_workqueue("mod_stat_scheduler");
    if (!stat_workqueue) {
        printk(KERN_ERR "Failed to create workqueue\n");
        return -ENOMEM;
    }

    INIT_DELAYED_WORK(&stat_work, work_handler);
    queue_delayed_work(stat_workqueue, &stat_work, msecs_to_jiffies(delay_ms));

    return 0;
}

static void __exit mod_stat_scheduler_exit(void) {
    cancel_delayed_work_sync(&stat_work);
    flush_workqueue(stat_workqueue);
    destroy_workqueue(stat_workqueue);
    printk(KERN_INFO "mod_stat_scheduler module unloaded\n");
}

module_init(mod_stat_scheduler_init);
module_exit(mod_stat_scheduler_exit);

module_param(delay_ms, uint, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(delay_ms, "Delay in milliseconds for periodic statistics output");