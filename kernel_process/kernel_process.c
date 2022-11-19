/* 모듈 head 파일 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL"); /* License 선언 */
MODULE_DESCRIPTION("Hello module"); /* 모듈 설명 */  

/* 모듈 로드시 수행하는, 초기 함수 */
static int process_init(void)
{

	int total_process = 0; /* process count */
	struct task_struct *task_list;

    /* 커널에 정의된, printf 같은 기능 */ 
    printk(KERN_INFO "Hello, Process List initialized\n");

	for_each_process(task_list) {
		printk(KERN_INFO "Process: %s, PID: %d\n", task_list->comm, task_list->pid); /* task->comm: task name */
		printk(KERN_INFO "  state: %4d, priority       : %d\n", task_list->__state, task_list->prio);
		printk(KERN_INFO "  CPU  : %4d, schedule policy: %d\n", task_list->on_cpu, task_list->policy);
		
		total_process++;
	}

	printk(KERN_INFO "total process count: %d\n", total_process); /* total process count */

	printk(KERN_INFO "Current Process: %s, PID: %d\n", current->comm, current->pid); /* task->comm: task name */
	printk(KERN_INFO "	state: %4d, priority	   : %d\n", current->__state, current->prio);
	printk(KERN_INFO "	CPU  : %4d, schedule policy: %d\n", current->on_cpu, current->policy);
	
    return 0;
}

/* 모듈 제거시 수행하는, 종료 함수 */
static void process_exit(void)
{
    printk(KERN_INFO "Goodbye, Process List exited\n");
}

module_init(process_init); /* 모듈 함수 초기화 */
module_exit(process_exit); /* 모듈 함수 종료 */
