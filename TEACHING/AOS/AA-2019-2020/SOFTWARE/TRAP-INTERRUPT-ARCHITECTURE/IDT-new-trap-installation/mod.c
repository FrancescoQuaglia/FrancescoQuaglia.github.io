#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kprobes.h>

#include "control.h"

extern void my_handler(void);

static __init int my_irq_init(void) {

	int err = 0;

	printk("%s: module init\n",MOD_NAME);

	err = setup_my_irq();
	if (err) goto out;

out:
	return err;
}// my irq init

void __exit my_irq_exit(void)
{

	cleanup_my_irq();

	pr_info("%s: module exit\n",MOD_NAME);

}

module_init(my_irq_init);
module_exit(my_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia");