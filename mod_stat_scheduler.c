/*
 * Beispiel zur Ausführung einer Funktion alle x Millisekunden.
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rainer Keller");
MODULE_DESCRIPTION("Modul zur periodischen Ausführung des work_handler");

#define DEFAULT_DELAY_MS 1000 // Standardverzögerung in Millisekunden

static unsigned long delay_ms = DEFAULT_DELAY_MS; // Parameter für die Verzögerung in Millisekunden

module_param(delay_ms, ulong, 0644); // Definition von delay_ms als Modulparameter
MODULE_PARM_DESC(delay_ms, "Verzögerung in Millisekunden für die periodische Ausführung des work_handler");

/*
 * Funktionsdefinitionen
 */
static void work_handler(struct work_struct *w);

/*
 * Daten-Deklarationen
 */
static struct workqueue_struct *wq = NULL;
static DECLARE_DELAYED_WORK(dwork, work_handler);

static void work_handler(struct work_struct *w) {
    static int times = 0;
    struct task_struct *task = current; // Aktuellen Prozess abrufen
    int latency_record_count = 5; // Anzahl der Latenzdatensätze
    unsigned long latency_record[5]; // Array für Latenzdatensätze
    int i;

    // Latenzen des aktuellen Prozesses abrufen
    for (i = 0; i < latency_record_count; ++i) {
        latency_record[i] = task->latency_record.latency[i];
    }

    printk(KERN_DEBUG "work_handler läuft w:%pX das %d. Mal\n", w, times++); // pr_debug() verwenden
    printk(KERN_DEBUG "Latenzdaten für den aktuellen Prozess:\n");
    for (i = 0; i < latency_record_count; ++i) {
        printk(KERN_DEBUG "Latenz %d: %lu\n", i, latency_record[i]);
    }
    printk(KERN_DEBUG "-----------------------------------------\n");

    queue_delayed_work(wq, &dwork, msecs_to_jiffies(delay_ms));
}

static int __init init_module(void) {
    printk(KERN_DEBUG "Hallo vom Modul\n"); // pr_debug() verwenden
    wq = alloc_workqueue("test", WQ_UNBOUND, 1);
    if (NULL == wq) {
        pr_err("Kann keine Arbeitswarteschlange zuweisen");
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
