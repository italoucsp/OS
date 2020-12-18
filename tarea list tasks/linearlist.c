#include <linux/sched.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

int init_module(void) {
	struct task_struct *task;	
	for_each_process(task) {
		printk(KERN_INFO "command: %s\nstate: %ld\npid: %d\n", task->comm, task->state, task->pid);
	}
	return 0;
}

void exit_module(void) {
	printk(KERN_INFO "Revoming module.\n");
}

module_init(init_module);
module_exit(exit_module);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linear for Listing task OS");
MODULE_AUTHOR("Italo");