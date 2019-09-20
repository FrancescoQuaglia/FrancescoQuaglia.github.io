
/*  
 *  broadcast dev file driver - you can broadcast streams of bytes on all the registered ttys 
 *  by simply writing to the broadcast dev file
 *  preliminary you need to mknod the dev file and assign the major retrived while mounting this module
 */

#define EXPORT_SYMTAB
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sched.h>	
#include <linux/pid.h>		/* For pid types */
#include <linux/tty.h>		/* For the tty declarations */
#include <linux/version.h>	/* For LINUX_VERSION_CODE */


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia");


static ssize_t print_stream_everywhere(char *, size_t );

static int broadcast_open(struct inode *, struct file *);
static int broadcast_release(struct inode *, struct file *);
static ssize_t broadcast_write(struct file *, const char *, size_t, loff_t *);

#define DEVICE_NAME "broadcast"  /* Device file name in /dev/ - not mandatory  */


static int Major;            /* Major number assigned to broadcast device driver */

//registered ttys
char * targets[] = {	"/dev/pts/1",
		 	"/dev/pts/2",
			"/dev/pts/3",
			"/dev/pts/4",
			NULL
		};



/* the actual driver */


static int broadcast_open(struct inode *inode, struct file *file)
{

//device opened by a default nop
   return 0;
}


static int broadcast_release(struct inode *inode, struct file *file)
{

//device closed by default nop
   return 0;

}



static ssize_t broadcast_write(struct file *filp,
   const char *buff,
   size_t len,
   loff_t *off)
{
   printk("somebody called a write on broadcast dev with [major,minor] number [%d,%d]\n",MAJOR(filp->f_dentry->d_inode->i_rdev),MINOR(filp->f_dentry->d_inode->i_rdev));
   return print_stream_everywhere(buff, len);
}



static struct file_operations fops = {
  .write = broadcast_write,
  .open =  broadcast_open,
  .release = broadcast_release
};


static ssize_t print_stream_everywhere(char *stream, size_t size )
{
	int i;
	struct file * f = NULL;
	printk("print stream function of broadcast dev called - tageting all registered ttys\n");

	for (i = 0; targets[i] != NULL; i++){
		printk("tageting %s\n",targets[i]);
        	f = filp_open(targets[i],O_WRONLY,0666);	
		if(IS_ERR(f)) continue;
		vfs_write(f,stream,size,&f->f_pos);
		filp_close(f,NULL);

	}

out:
	return size;


}



int init_module(void)
{

	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0) {
	  printk("Registering noiser device failed\n");
	  return Major;
	}

	printk(KERN_INFO "Broadcast device registered, it is assigned major number %d\n", Major);


	return 0;
}

void cleanup_module(void)
{

	unregister_chrdev(Major, DEVICE_NAME);

	printk(KERN_INFO "Broadcast device unregistered, it was assigned major number %d\n", Major);
}
