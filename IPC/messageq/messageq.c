#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/timer.h>
#include <linux/gpio.h> /* gpio functions */
#include <linux/kfifo.h>

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

#define FIFO_SIZE 1024  // Message FIFO 크기 설정

static DECLARE_KFIFO(shared_fifo, char, FIFO_SIZE);

static struct task_struct *thread1;
static struct task_struct *thread2;

static struct mutex my_mutex;    // Mutex 선언

static int shared_counter = 0;

// 스레드 함수
static int thread_function1(void *arg) {
    int i;
    char message[] = "Hello from Thread 1!";

    for (i = 0; i < 5; i++) {
       
        // Mutex 획득
        if (mutex_lock_interruptible(&my_mutex)) {
            pr_err("Failed to acquire mutex in thread %s\n", (char *)arg);
            return -1;
        }
        printk(KERN_INFO "Thread %s acquired the mutex\n", (char *)arg);

        kfifo_in(&shared_fifo, message, sizeof(message));
        printk(KERN_INFO "Thread 1: Sent message %d - %s\n", i+1, message);

        msleep(1000); // 1초 간격
   
       // 공유 자원 접근
        shared_counter++;
        printk(KERN_INFO "Thread %s incremented shared_counter to: %d\n", (char *)arg, shared_counter);

      // GPIO 18: LED 켜기
            writel(1 << 18, gpio_base + GPIO_SET_OFFSET_18);
         
        // 잠시 대기
        msleep(1000);       
        
       // GPIO 18: LED 끄기
           writel(1 << 18, gpio_base + GPIO_CLR_OFFSET_18);

       // Mutex 해제
        printk(KERN_INFO "Thread %s released the mutex\n", (char *)arg);
        mutex_unlock(&my_mutex);

        msleep(500);
    }
    return 0;
}


// 스레드 함수
static int thread_function2(void *arg) {
    int i;
    char buffer[100];      

    for (i = 0; i < 5; i++) {

        // Mutex 획득
        if (mutex_lock_interruptible(&my_mutex)) {
            printk(KERN_INFO "Failed to acquire mutex in thread %s\n", (char *)arg);
            return -1;
        }
        printk(KERN_INFO "Thread %s acquired the mutex\n", (char *)arg);

       if (kfifo_out(&shared_fifo, buffer, sizeof(buffer))) {
            printk(KERN_INFO "Thread 2: Received message %d - %s\n", i+1, buffer);
        } else {
            printk(KERN_INFO "Thread 2: FIFO empty, waiting...\n");
        }
    
        // 공유 자원 접근
        shared_counter++;
        printk(KERN_INFO "Thread %s incremented shared_counter to: %d\n", (char *)arg, shared_counter);

      // GPIO 23: LED 켜기
            writel(1 << 23, gpio_base + GPIO_SET_OFFSET_23);
        
        // 잠시 대기
        msleep(1000);       
        
         // GPIO 23: LED 끄기
           writel(1 << 23, gpio_base + GPIO_CLR_OFFSET_23);

        // Mutex 해제
        printk(KERN_INFO "Thread %s released the mutex\n", (char *)arg);
        mutex_unlock(&my_mutex);

        msleep(500);
    }
    return 0;
}

static int __init message_queue_init(void) {
    int i;

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

    printk(KERN_INFO "Initializing message queue...\n");

    // Message FIFO 초기화
    INIT_KFIFO(shared_fifo);

    // Mutex 초기화
    mutex_init(&my_mutex);

    // 커널 스레드 생성
    thread1 = kthread_run(thread_function1, "Thread 1", "thread1");
    thread2 = kthread_run(thread_function2, "Thread 2", "thread2");

    if (IS_ERR(thread1) || IS_ERR(thread2)) {
        pr_err("Failed to create threads\n");
        return -1;
    }

    return 0;
}

static void __exit message_queue_exit(void) {

// 메모리 매핑 해제
    if (gpio_base)
        iounmap(gpio_base);

   printk(KERN_INFO "Exitng message queue...\n");
}

module_init(message_queue_init);
module_exit(message_queue_exit);