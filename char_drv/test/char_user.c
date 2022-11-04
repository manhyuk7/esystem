#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_BUFFER 26 /* Alphabet size */
char buf_in[MAX_BUFFER];
char buf_out[MAX_BUFFER];

int main(void)
{
    int fd, i;
 
    /* open device driver : /dev/char_dd */   
    fd = open("/dev/char_dd", O_RDWR);
    if (fd < 0) 
        printf("/dev/char_dd open failed\n");

    /* buf_in : AAA..., buf_out : ABC... */
    for(i = 0; i < MAX_BUFFER; i++) {
      buf_in[i] = 'A';  
      buf_out[i] = 'A' + i;
    }

    /* print buf_in before kernel write */
    for(i = 0; i < MAX_BUFFER; i++) 
	fprintf(stdout, "%c", buf_in[i]);
    fprintf(stdout, "\n");

    write(fd, buf_out, MAX_BUFFER); /* write buf_out to char driver */
    read(fd, buf_in, MAX_BUFFER);   /* read buf_in from char driver */

    /* print buf_in */
    for(i = 0; i < MAX_BUFFER; i++)
	fprintf(stdout, "%c", buf_in[i]);
    fprintf(stdout, "\n");

    close(fd);
    return 0;
}