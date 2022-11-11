#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_BUFFER 40 /* Alphabet size */
char buf_in[MAX_BUFFER];

int main(void)
{
    int fd, i;
 
    /* open device driver : /dev/switch_dd */   
    fd = open("/dev/switch_dd", O_RDWR);

    if (fd < 0) 
        printf("/dev/switch_dd open failed\n");

    read(fd, buf_in, MAX_BUFFER);   /* read buf_in from char driver */

    printf("irq number = %d, irq count = %d\n", buf_in[0], buf_in[1]);
	
    close(fd);
    return 0;
}