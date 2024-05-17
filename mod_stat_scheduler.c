#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yannick Schilling");
MODULE_DESCRIPTION("Kernel module for printing latency statistic periodically");
MODULE_PARM_DESC(delay_ms, "Delay in ms defining the interval of the output");

#define DEFAULT_DELAY_MS 1000

static struct delayed_work mod_work; 
static int delay_ms = DEFAULT_DELAY_MS; // param can be manually adjusted when using insmod 

// function being called
static void work_handler(struct work_struct *work) {
    struct task_struct *task = current; // get current task (current is macro) 
    unsigned int i;

    // print out stats
    printk(KERN_INFO "Latency record:\n"); 
    for (i = 0; i < task->latency_record_count; i++) { // latency_record_count in latencytop.h
        printk(KERN_INFO "count: %u, backtrace[0]: %pX\n", task->latency_record[i].count, (void*)task->latency_record[i].backtrace[0]);
    }

    // scedule next work with the assigned delay
    schedule_delayed_work(&mod_work, msecs_to_jiffies(delay_ms)); // convert jiffies
}


__init int init_module(void) {
    printk(KERN_INFO "Module mod_stat_scheduler started\n");

    // init deladyed work queue (form Kernel workqueue.h)
    INIT_DELAYED_WORK(&mod_work, work_handler); 
    // schedule first work
    schedule_delayed_work(&mod_work, msecs_to_jiffies(delay_ms));

    return 0; 
}


__exit void cleanup_module(void) {
    // cancel complete work_queue
    cancel_delayed_work_sync(&mod_work);
    printk(KERN_INFO "Module stat_scheduler exited\n");
}

module_param(delay_ms, int, S_IRUGO); // module param, including rights
