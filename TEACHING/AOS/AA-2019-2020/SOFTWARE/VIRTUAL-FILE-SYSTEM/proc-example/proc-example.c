#define EXPORT_SYMTAB
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/pid.h>          /* For pid types */
#include <linux/tty.h>          /* For the tty declarations */
#include <linux/version.h>      /* For LINUX_VERSION_CODE */
#include <linux/signal_types.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>


int len;

void create_new_proc_entry(void) ;

char *msg;

struct proc_dir_entry * my_entry;

ssize_t read_proc(struct file *filp,char *buf, size_t count, loff_t *offp ) {

     int ret;
     int temp = len;

	if(*offp >= len) return 0;

	temp = len - *offp;

	if(count>temp) {
		count=temp;
	}
	ret = copy_to_user(buf,msg+(*offp), count);//the destination buffer is still user land

	*offp += count;//try what happens here
   
	return count - ret;
}

struct file_operations proc_fops = {
	owner: THIS_MODULE,
	read: read_proc
};

void create_new_proc_entry() {

	my_entry = proc_create("hello",0,NULL,&proc_fops);

	msg="Hello World\n";
	len=strlen(msg);
	printk(KERN_INFO "1.len=%d",len);
}


int proc_init (void) {
 create_new_proc_entry();
 return 0;
}

void proc_cleanup(void) {
 remove_proc_entry("hello",NULL);
}

MODULE_LICENSE("GPL"); 
module_init(proc_init);
module_exit(proc_cleanup);
