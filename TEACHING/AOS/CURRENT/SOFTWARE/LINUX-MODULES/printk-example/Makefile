
obj-m += the_printk-example.o
the_printk-example-objs += printk-example.o lib/scth.o
#the_virtual-to-physical-memory-mapper-objs += virtual-to-physical-memory-mapper.o 


A = $(shell cat /sys/module/the_usctm/parameters/sys_call_table_address)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

mount:
	insmod the_printk-example.ko the_syscall_table=$(A)
