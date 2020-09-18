#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define PROC_NAME "jiffies"
#define BUFFER_SIZE 128

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;

    if (completed) {
        completed = 0;
        return 0;
    }
    completed = 1;
    rv = sprintf(buffer, "Current jiffies: %lu.\n",jiffies);
    copy_to_user(usr_buf, buffer, rv);
    return rv;
}

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
};


int jiffi_init(void)
{
    proc_create(PROC_NAME, 0, NULL, &proc_ops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}
void jiffi_exit(void)
{
    printk(KERN_INFO "Removing jiffies module.\n");
    remove_proc_entry(PROC_NAME, NULL);
}

module_init(jiffi_init);
module_exit(jiffi_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Modulo Jiffies");
MODULE_AUTHOR("Italo");