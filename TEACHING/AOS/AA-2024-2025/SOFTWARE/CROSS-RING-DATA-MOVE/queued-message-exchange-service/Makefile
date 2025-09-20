
obj-m += the_message_exchange_service.o
the_message_exchange_service-objs += message_exchange_service_with_intermediate_buffering.o lib/scth.o

A = $(shell cat /sys/module/the_usctm/parameters/sys_call_table_address)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

mount:
	insmod the_message_exchange_service.ko the_syscall_table=$(A)
