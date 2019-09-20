
/*  
 *  address space hit dev file driver - you can read memory location of target processes by writing/reading to this 
 *   device file 
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

#define AUDIT if(1)

static int hit_open(struct inode *, struct file *);
static int hit_release(struct inode *, struct file *);
static ssize_t hit_write(struct file *, const char *, size_t, loff_t *);
static ssize_t hit_read(struct file *, char *, size_t, loff_t *);

#define DEVICE_NAME "ADDRESS SPACE READER"  /* Device file name in /dev/ - not mandatory  */


static int Major;            /* Major number assigned to broadcast device driver */


/* the actual driver */


static int hit_open(struct inode *inode, struct file *file)
{

//device opened by a default nop
   return 0;
}


static int hit_release(struct inode *inode, struct file *file)
{

//device closed by default nop
   return 0;

}

#define LINE_SIZE 128

//#define PAGE_SIZE 4096

#define NO (0)
#define YES (NO+1)

char the_page[PAGE_SIZE];
int valid = 0;

#define PML4(addr) (((long long)(addr) >> 39) & 0x1ff)
#define PDP(addr) (((long long)(addr) >> 30) & 0x1ff)
#define PDE(addr) (((long long)(addr) >> 21) & 0x1ff)
#define PTE(addr) (((long long)(addr) >> 12) & 0x1ff)

static ssize_t hit_read(struct file *filp,  char *buff, size_t len, loff_t *off) {

	int ret;

	if (*off >= PAGE_SIZE || valid == NO) return 0;
	if (len > PAGE_SIZE - *off) len = PAGE_SIZE - *off;

	//copy to user the logged page
	ret = copy_to_user((void*)buff,(void*)the_page,len);

	*off = len - ret;
	return len - ret;

}

static ssize_t hit_write(struct file *filp, const char *buff, size_t len, loff_t *off) {

   char buffer[LINE_SIZE];
   long pid;
   unsigned long addr;
   int format = 0;
   int i,j;
   int ret;

   char* args[3];

   char* target_page;

   struct task_struct *the_task;

   void ** my_pdp;
   void ** my_pd;
   void ** my_pde;
   void ** my_pte;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
   printk("%s: somebody called a read on hit dev with [major,minor] number [%d,%d]\n",DEVICE_NAME,MAJOR(filp->f_inode->i_rdev),MINOR(filp->f_inode->i_rdev));
#else
   printk("%s: somebody called a read on hit dev with [major,minor] number [%d,%d]\n",DEVICE_NAME,MAJOR(filp->f_dentry->d_inode->i_rdev),MINOR(filp->f_dentry->d_inode->i_rdev));
#endif

  if(len >= LINE_SIZE) return -1;
  ret = copy_from_user((void*)buffer,(void*)buff,len);

  j = 1;
  for(i=0;i<len;i++){
	if(buffer[i] == ' ') {
		buffer[i] = '\0';
		args[j++] = buffer + i + 1;
		format++;
	}
  }

  if(format != 1) return -1;

  args[0] = buffer;

  buffer[len] = '\0';
  ret = kstrtol(args[0],10,&pid);
  ret = kstrtol(args[1],10,&addr);

  printk("%s: args are: %ld - %lu\n",DEVICE_NAME,pid,addr);

  the_task = pid_task(find_vpid(pid),PIDTYPE_PID);

  if(!the_task) return -1;

  if(!(the_task->mm)){
	ret = -1; 
	goto out;
  }
  atomic_inc(&(the_task->mm->mm_count));//to be released in failures of below parts

  if(!(the_task->mm->pgd)){ 
	ret = -1; 
	goto mm_release;
  }
  spin_lock(&(the_task->mm->page_table_lock));//to be released in failures of below parts

  AUDIT
  printk("%s: process %ld is there with its memory map\n",DEVICE_NAME,pid);


  my_pdp = (void**)the_task->mm->pgd;

  if((ulong)my_pdp[PML4(addr)] & 0x1){
  	printk("%s: process %ld has PML4 mapped page at address %p\n",DEVICE_NAME,pid,(void*)addr);

	my_pd = __va((ulong)my_pdp[PML4(addr)] & 0x7ffffffffffff000);

	if((ulong)my_pd[PDP(addr)] & 0x1){
  		printk("%s: process %ld has PDP mapped page at address %p\n",DEVICE_NAME,pid,(void*)addr);
		my_pde = __va((ulong)my_pd[(ulong)PDP(addr)] & 0x7ffffffffffff000);

		if((ulong)my_pde[PDE(addr)] & 0x1){
  			printk("%s: process %ld has PDE mapped page at address %p\n",DEVICE_NAME,pid,(void*)addr);
			my_pte = __va((ulong)my_pde[PDE(addr)]& 0x7ffffffffffff000);

			if((ulong)my_pte[PTE(addr)] & 0x1){
  				printk("%s: process %ld has PTE mapped page at address %p\n",DEVICE_NAME,pid,(void*)addr);
				target_page =(char*)__va((ulong)my_pte[PTE(addr)]& 0x7ffffffffffff000);
				memcpy((char*)the_page,(char*)target_page,PAGE_SIZE);
				valid = YES;
				goto copy_done;
			}
		}
       }
  }

  AUDIT
  printk("%s: process %ld has no mapped page at address %p\n",DEVICE_NAME,pid,(void*)addr);
  valid = NO;

copy_done:

  spin_unlock(&(the_task->mm->page_table_lock));//to be released in failures of below parts

mm_release:
  atomic_dec(&(the_task->mm->mm_count));//to be released in failures of below parts

out:
  return len;

}


static struct file_operations fops = {
  .owner = THIS_MODULE,
  .write = hit_write,
  .read = hit_read,
  .open =  hit_open,
  .release = hit_release
};


int init_module(void) {

	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0) {
	  printk("Registering hit device failed\n");
	  return Major;
	}

	printk(KERN_INFO "%s: device registered, it is assigned major number %d\n", DEVICE_NAME, Major);


	return 0;
}

void cleanup_module(void) {

	unregister_chrdev(Major, DEVICE_NAME);

	printk(KERN_INFO "%s: device unregistered, it was assigned major number %d\n",DEVICE_NAME, Major);
}
