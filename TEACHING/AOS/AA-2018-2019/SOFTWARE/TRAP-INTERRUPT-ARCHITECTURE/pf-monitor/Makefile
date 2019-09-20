obj-y := service/ intercept/
CURRENT_PATH = $(shell pwd)
LINUX_KERNEL = $(shell uname -r)
LINUX_KERNEL_PATH = /usr/src/linux-headers-$(LINUX_KERNEL)
KDIR = $(LINUX_KERNEL_PATH) 
KVERSION=$(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build M=$(CURRENT_PATH)

install:
	insmod ./service/service_installer.ko
	cd intercept ; ./load.sh ; cd ..

uninstall:
	./intercept/unload.sh
	rmmod service_installer 
