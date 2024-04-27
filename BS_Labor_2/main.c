#include <linux/init.h>             // Header für Initialisierungsroutinen
#include <linux/module.h>           // Header für Kernelmodule
#include <linux/workqueue.h>        // Header für Workqueues

#define LATENCY_RECORD_COUNT 10     // Anzahl der Latenzdatensätze
static unsigned long latency_record[LATENCY_RECORD_COUNT] = {0}; // Array für Latenzdaten
static int current_index = 0;       // Aktueller Index im Latenzdaten-Array

static unsigned int delay_ms = 1000;    // Standardverzögerung von 1000 ms
module_param(delay_ms, uint, 0644);     // Kernelparameter für Verzögerung hinzufügen
MODULE_PARM_DESC(delay_ms, "Delay in milliseconds for latency output"); // Beschreibung des Parameters

static struct workqueue_struct *my_workqueue;    // Workqueue-Struktur
static struct delayed_work my_work;             // Verzögerte Arbeit

// Arbeitshandlerfunktion, um Latenzstatistiken auszugeben
static void work_handler(struct work_struct *work) {
    int i;

    // Ausgabe der Latenzstatistiken
    printk(KERN_INFO "Latency statistics:");
    for (i = 0; i < LATENCY_RECORD_COUNT; ++i) {
        printk(KERN_INFO "Latency[%d]: %lu ms", i, latency_record[i]);    // Ausgabe jedes Latenzdatensatzes
    }

    // Planen Sie die Arbeit erneut
    queue_delayed_work(my_workqueue, &my_work, msecs_to_jiffies(delay_ms));   // Arbeit für zukünftige Ausführung planen
}

static int __init init_module(void) {
    printk(KERN_INFO "Module initialized\n");   // Kernel-Log: Modulinitialisierungsnachricht

    // Initialisieren und planen Sie die verzögerte Arbeit
    my_workqueue = create_workqueue("my_workqueue");   // Workqueue erstellen
    INIT_DELAYED_WORK(&my_work, work_handler);    // Verzögerte Arbeit initialisieren
    queue_delayed_work(my_workqueue, &my_work, msecs_to_jiffies(delay_ms)); // Verzögerte Arbeit planen

    return 0;   // Erfolgreiche Initialisierung
}

static void __exit cleanup_module(void) {
    // Löschen Sie die Arbeit und die Warteschlange beim Beenden des Moduls
    cancel_delayed_work_sync(&my_work); // Verzögerte Arbeit abbrechen
    destroy_workqueue(my_workqueue);    // Workqueue zerstören

    printk(KERN_INFO "Module exited\n");  // Kernel-Log: Modulbeendigungsnachricht
}

MODULE_LICENSE("GPL");  // Lizenzinformationen für das Modul
module_init(init_module);   // Initialisierungsfunktion für das Modul
module_exit(cleanup_module);    // Aufräumfunktion für das Modul
