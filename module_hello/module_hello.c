/* 모듈 head 파일 */
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL"); /* License 선언 */
MODULE_DESCRIPTION("Hello module"); /* 모듈 설명 */  

/* 모듈 로드시 수행하는, 초기 함수 */
static int hello_init_module(void)
{
    /* 커널에 정의된, printf 같은 기능 */ 
    printk(KERN_INFO "Hello, Module is initialized\n");
    return 0;
}

/* 모듈 제거시 수행하는, 종료 함수 */
static void hello_exit_module(void)
{
    printk(KERN_INFO "Goodbye, Module is exited\n");
}

module_init(hello_init_module); /* 모듈 함수 초기화 */
module_exit(hello_exit_module); /* 모듈 함수 종료 */