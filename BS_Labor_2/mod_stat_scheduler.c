#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yannick Schilling"); 
MODULE_DESCRIPTION("Module to periodically display work handler stats");


//dummy
static void work_handler(struct work_struct *w);

#define LATENCY_RECORD_COUNT 10     
static unsigned long latency_record[LATENCY_RECORD_COUNT] = {0}; // array for latency datasets
static int current_index = 0;       

// parameter
static unsigned int delay_ms = 1000;    // defaul delay of 1000 ms
static DECLARE_DELAYED_WORK(my_work, work_handler);
MODULE_PARM_DESC(delay_ms, "Delay in milliseconds for latency output"); // descrition for modinfo

static struct workqueue_struct *my_workqueue = NULL;    // Workqueue-Struktur
static struct delayed_work my_work;             // Verzögerte Arbeit

// work handler function, to display stats
static void work_handler(struct work_struct *work) {
    // output
    printk(KERN_INFO "Latency statistics:");
    for (int i = 0; i < LATENCY_RECORD_COUNT; ++i) {
        printk(KERN_INFO "Latency[%d]: %lu ms", i, latency_record[i]);    
    }

    // plan work again
    queue_delayed_work(my_workqueue, &my_work, msecs_to_jiffies(delay_ms));   
}

__init int  init_module(void) {
    printk(KERN_INFO "Module initialized\n");   

    // 
    my_workqueue = create_workqueue("my_workqueue");   // create workqueue
    INIT_DELAYED_WORK(&my_work, work_handler);    // init delayed work
    queue_delayed_work(my_workqueue, &my_work, msecs_to_jiffies(delay_ms)); // plan delayed work

    return 0;  
}

__exit void cleanup_module(void) {    
    cancel_delayed_work_sync(&my_work); 
    destroy_workqueue(my_workqueue);    

    printk(KERN_INFO "Module exited\n");  // Kernel-Log: Modulbeendigungsnachricht
}



