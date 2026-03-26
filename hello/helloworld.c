#include <linux/init.h>
#include <linux/module.h>

#include <linux/sched/signal.h>
#include <linux/sched/task.h>
#include <linux/sched/mm.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/oom.h>

MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
	struct task_struct *task;
	printk(KERN_ALERT "Hello, world\n");
        for_each_process(task) {
   		printk("%s[%d]\n", task->comm, task->pid);
	}
	return 0;
}
static void hello_exit(void)
{
	printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(hello_init);
module_exit(hello_exit);
