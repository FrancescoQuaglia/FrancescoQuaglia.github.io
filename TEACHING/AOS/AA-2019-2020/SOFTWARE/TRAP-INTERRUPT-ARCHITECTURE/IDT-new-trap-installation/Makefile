obj-m := trap_installer.o
trap_installer-objs :=  control.o  mod.o

KDIR ?= /lib/modules/$(shell uname -r)/build


all:
	make -C $(KDIR) M=$(PWD) modules
