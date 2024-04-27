obj-m += mod_delayed_workqueue.o
obj-m += mod_stat_scheduler.o

KVERSION = $(shell uname -r)

modules all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
