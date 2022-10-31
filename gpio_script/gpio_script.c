/* gpio control script */
/*
$ echo "18" > /sys/class/gpio/export
$ echo "out" > /sys/class/gpio/gpio18/direction
$ echo "1" > /sys/class/gpio/gpio18/value
$ echo "0" > /sys/class/gpio/gpio18/value
$ echo "18" > /sys/class/gpio/unexport
*/

#include <stdio.h>
#include <stdlib.h> /* atoi( ) 함수 */
#include <string.h>
#include <fcntl.h>
#include <unistd.h> /* close( ) 함수 */

int ledControl(int gpio)
{
    int fd, i;
    char buf[BUFSIZ];

    /* echo "18" > /sys/class/gpio/export */
    fd = open("/sys/class/gpio/export", O_WRONLY); /* 해당 GPIO 디바이스 사용 준비 */
    sprintf(buf, "%d", gpio);
    write(fd, buf, strlen(buf));
    close(fd);

    /* echo "out" > /sys/class/gpio/gpio18/direction */
    sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio); /* 해당 GPIO 디바이스의 방향 설정 */
    fd = open(buf, O_WRONLY);
    /* LED를 위해 디바이스를 출력으로 설정 : 입력의 경우 write(fd, "in", 3); 사용 */
    write(fd, "out", 4);
    close(fd);

    for(i = 0; i < 5; i++) {
        /*echo "1" > /sys/class/gpio/gpio18/value */
        sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio); /* 디바이스에 값 출력: LED on */
        fd = open(buf, O_WRONLY);
        write(fd, "1", 2);
        close(fd); 

        sleep(1);

        /* echo "0" > /sys/class/gpio/gpio18/value */
        sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio); /* 디바이스에 값 출력: LED off */
        fd = open(buf, O_WRONLY);
        write(fd, "0", 2);
        close(fd);

       sleep(1);
    }


    /* echo "18" > /sys/class/gpio/unexport */
    fd = open("/sys/class/gpio/unexport", O_WRONLY); /* 사용한 GPIO 디바이스 해제하기 */
    sprintf(buf, "%d", gpio);
    write(fd, buf, strlen(buf));
    close(fd);
    
    return 0;
}

int main(int argc, char **argv)
{
    int gno;

    if(argc < 2) {
        printf("Usage : %s GPIO_NO\n", argv[0]);
    return -1;
    }
    
    gno = atoi(argv[1]);
    ledControl(gno);
    return 0;
}