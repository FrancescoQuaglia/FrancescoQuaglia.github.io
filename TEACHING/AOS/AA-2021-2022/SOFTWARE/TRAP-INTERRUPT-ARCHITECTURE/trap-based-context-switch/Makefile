obj-m := trap_installer.o
trap_installer-objs :=  control.o  mod.o lib/scth.o

KDIR ?= /lib/modules/$(shell uname -r)/build

A = $(shell cat /sys/module/the_usctm/parameters/sys_call_table_address)

all:
	make -C $(KDIR) M=$(PWD) modules

mount:
	insmod trap_installer.ko the_syscall_table=$(A)

reset:
	echo "0" > /sys/module/trap_installer/parameters/the_pid

