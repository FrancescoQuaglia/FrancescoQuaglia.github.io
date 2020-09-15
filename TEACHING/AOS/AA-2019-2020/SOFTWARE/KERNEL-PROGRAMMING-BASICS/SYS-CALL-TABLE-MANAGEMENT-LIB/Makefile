
obj-m += example_usage_module.o
example_usage_module-objs += example-usage.o lib/scth.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

