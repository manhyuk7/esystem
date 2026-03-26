#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/timer.h>
#include <linux/gpio.h> /* gpio functions */

MODULE_LICENSE("GPL");

#define GPIO_BASE 0xFE200000    /* 라즈베리파이 4의 GPIO 베이스 주소 */

#define GPIO_SET_OFFSET_18 0x1C    /* LED 18 : SET Offset */
#define GPIO_CLR_OFFSET_18 0x28    /* LED 18:  Clear Offset */
#define GPIO_SEL_OFFSET_18 0x04    /* LED 18:  Selection Offset */

#define GPIO_SET_OFFSET_23 0x1C    /* LED 23 : SET Offset */
#define GPIO_CLR_OFFSET_23 0x28    /* LED 23:  Clear Offset */
#define GPIO_SEL_OFFSET_23 0x08    /* LED 23:  Selection Offset */

volatile void __iomem *gpio_base;
unsigned int sel_val;

static struct task_struct *thread1;
static struct task_struct *thread2;
static struct semaphore my_semaphore; // 세마포어 정의
static int shared_counter = 0;

// 스레드 함수
static int thread_function1(void *arg) {
    int i;

    for (i = 0; i < 5; i++) {
        // 세마포어 획득 (다운)
        if (down_interruptible(&my_semaphore)) {
            pr_err("Failed to acquire semaphore in thread %s\n", (char *)arg);
            return -1;
        }

        pr_info("Thread %s acquired the semaphore\n", (char *)arg);

        // 공유 자원 접근
        shared_counter++;
        pr_info("Thread %s incremented shared_counter to: %d\n", (char *)arg, shared_counter);

        // GPIO 18: LED 켜기
            writel(1 << 18, gpio_base + GPIO_SET_OFFSET_18);
         
        // 잠시 대기
        msleep(1000);       
        
       // GPIO 18: LED 끄기
           writel(1 << 18, gpio_base + GPIO_CLR_OFFSET_18);

        // 세마포어 해제 (업)
        pr_info("Thread %s released the semaphore\n", (char *)arg);
        up(&my_semaphore);

        msleep(500);
    }
    return 0;
}

// 스레드 함수
static int thread_function2(void *arg) {
    int i;
    for (i = 0; i < 5; i++) {
        // 세마포어 획득 (다운)
        if (down_interruptible(&my_semaphore)) {
            pr_err("Failed to acquire semaphore in thread %s\n", (char *)arg);
            return -1;
        }

        pr_info("Thread %s acquired the semaphore\n", (char *)arg);

        // 공유 자원 접근
        shared_counter++;
        pr_info("Thread %s incremented shared_counter to: %d\n", (char *)arg, shared_counter);

        // GPIO 23: LED 켜기
            writel(1 << 23, gpio_base + GPIO_SET_OFFSET_23);
        
        // 잠시 대기
        msleep(1000);       
        
         // GPIO 23: LED 끄기
           writel(1 << 23, gpio_base + GPIO_CLR_OFFSET_23);

        // 세마포어 해제 (업)
        pr_info("Thread %s released the semaphore\n", (char *)arg);
        up(&my_semaphore);
 
        msleep(500);
    }
    return 0;
}

static int __init semaphore_module_init(void) {

    /* gpio memory mapping */
    gpio_base = ioremap(GPIO_BASE, 0x100);
    if (!gpio_base) {
        pr_err("Failed to map GPIO memory\n");
        return -ENOMEM;
    }

    // GPIO 핀 설정 (예: GPIO 18을 출력으로 설정)
    sel_val = readl(gpio_base + GPIO_SEL_OFFSET_18);
    sel_val &= ~(7 << 24);            // GPIO 18 위치 초기화
    sel_val |= (1 << 24);                // GPIO 18을 출력으로 설정
    writel(sel_val, gpio_base + GPIO_SEL_OFFSET_18);

  // GPIO 핀 설정 (예: GPIO 23을 출력으로 설정)
    sel_val = readl(gpio_base + GPIO_SEL_OFFSET_23);
    sel_val &= ~(7 << 9);            // GPIO 23 위치 초기화
    sel_val |= (1 << 9);                // GPIO 23 을 출력으로 설정
    writel(sel_val, gpio_base + GPIO_SEL_OFFSET_23);

    printk(KERN_INFO "LED: sel_val %x\n", sel_val);

    pr_info("Semaphore module initialized\n");

    // 세마포어 초기화 (초기 값: 1)
    sema_init(&my_semaphore, 1);

    // 커널 스레드 생성
    thread1 = kthread_run(thread_function1, "Thread 1", "thread1");
    thread2 = kthread_run(thread_function2, "Thread 2", "thread2");

    if (IS_ERR(thread1) || IS_ERR(thread2)) {
        pr_err("Failed to create threads\n");
        return -1;
    }

    return 0;
}

static void __exit semaphore_module_exit(void) {
  // 메모리 매핑 해제
    if (gpio_base)
        iounmap(gpio_base);

    pr_info("Semaphore module exited\n");
}

module_init(semaphore_module_init);
module_exit(semaphore_module_exit);