obj-m := lwt.o
lwt-objs :=  control.o  mod.o

KDIR ?= /lib/modules/$(shell uname -r)/build


all:
	make -C $(KDIR) M=$(PWD) modules
