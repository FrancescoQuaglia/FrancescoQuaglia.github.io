
obj-m += the_page_table_auditor.o
the_page_table_auditor-objs += page_table_auditor.o lib/scth.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

