obj-m := hop.o
hop-objs :=  control.o  mod.o

KDIR ?= /lib/modules/$(shell uname -r)/build


all:
	make -C $(KDIR) M=$(PWD) modules
