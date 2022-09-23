
obj-m += function_pointer_exposer.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

activate:
	cat /sys/module/function_pointer_exposer/parameters/the_hook > /sys/module/hook/parameters/hook_func 

deactivate:
	echo "0" > /sys/module/hook/parameters/hook_func	
	
