#include <linux/sched.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

void dfs(struct task_struct *task) {
	printk(KERN_INFO "command: %s\nstate: %ld\npid: %d\n", task->comm, task->state, task->pid);
	struct task_struct *child;
	struct list_head *list;
	list_for_each(list, &task->children) {
		child = list_entry(list, struct task_struct, sibling);
		dfs(child);
	}
}

int init_module(void) {
	dfs(&init_task);
	return 0;
}

void exit_module(void) {
	printk(KERN_INFO "Revoming module.\n");
}

module_init(init_module);
module_exit(exit_module);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("DFS for Listing task OS");
MODULE_AUTHOR("Italo");