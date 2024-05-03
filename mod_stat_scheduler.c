#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");          
MODULE_AUTHOR("Name"); 
MODULE_DESCRIPTION("Kernel module for periodic statistics output"); 
MODULE_VERSION("0.1");         

#define DEFAULT_DELAY_MS 1000   

static struct delayed_work mod_work; 
static int delay_ms = DEFAULT_DELAY_MS; 

// Funktion, die beim Abarbeiten der Arbeitsschlange aufgerufen wird
static void work_handler(struct work_struct *work) {
    struct task_struct *task = current; // Aktuellen Task holen (current ist ein Makro)
    unsigned int i;

    // Ausgabe von Latenzstatistiken
    printk(KERN_INFO "latency_record:\n");
    for (i = 0; i < task->latency_record_count; i++) {
        printk(KERN_INFO "count: %u, backtrace[0]: %pX\n", 
               task->latency_record[i].count, 
               (void*)task->latency_record[i].backtrace[0]);
    }

    // Planen der nächsten Arbeit mit der definierten Verzögerung
    schedule_delayed_work(&mod_work, msecs_to_jiffies(delay_ms)); // msecs_to_jiffies konvertiert Millisekunden in Jiffies (Zeiteinheit des Kernels)
}

// Initialisierungsfunktion des Moduls
__init int init_module(void) {
    printk(KERN_INFO "Modul gestartet\n");

    // Initialisierung der verzögerten Arbeitsschlange
    INIT_DELAYED_WORK(&mod_work, work_handler); // Teil der Linux-Kernel-API (<linux/workqueue.h>)
    // Planen der ersten Arbeit mit der definierten Verzögerung
    schedule_delayed_work(&mod_work, msecs_to_jiffies(delay_ms));

    return 0; // Erfolgreiche Initialisierung
}

// Exit-Funktion des Moduls
__exit void cleanup_module(void) {
    // Synchrones Abbrechen der Arbeitsschlange und Warten auf deren Abschluss
    cancel_delayed_work_sync(&mod_work);
    printk(KERN_INFO "Modul beendet\n");
}

module_param(delay_ms, int, S_IRUGO); 
MODULE_PARM_DESC(delay_ms, "Delay in milliseconds for statistics output"); // Beschreibung des Parameters delay_ms
