#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/kdev_t.h>

#define LED_PATH "/dev/led_dd"

/* gpio_test 0 : led off */
/* gpio_test 1 : led on */

int main(int argc, char **argv){
    int fd = 0;
    
    /* ./a.out 1 or 0 */
    if(argc!=2){
    	printf("Usage: %s [LED binary]\n", argv[0]);
        exit(1);
    }
    
    /* LED_PATH: /dev/led_dd */
    if((fd = open(LED_PATH, O_RDWR | O_NONBLOCK)) < 0){
    	perror("open()");
        exit(1);
    }
    
    /* ./a.out 1 or 0 */
    write(fd, argv[1], strlen(argv[1]));
    
    close(fd);
    
    return 0;
}