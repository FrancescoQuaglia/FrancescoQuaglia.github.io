
obj-m += the_virtual-to-physical-memory-mapper.o
the_virtual-to-physical-memory-mapper-objs += virtual-to-physical-memory-mapper.o lib/scth.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

