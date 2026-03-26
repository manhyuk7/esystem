#include <stdio.h>
#include <unistd.h>

int main() {
    int pid;

    pid = fork();

    if(pid != 0) {   /* parent process */                                                             
          printf("I am a parent process \n");
          printf("    return pid = %d, parent pid = %d \n", pid, getpid()); 
   } else {                     /* child process */
          printf("I am a child process \n");
          printf("    return pid = %d, child  pid = %d \n", pid, getpid()); 
   }

  return 0;
}
