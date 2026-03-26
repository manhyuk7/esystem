#include <stdio.h>
#include <pthread.h>

// 스레드 함수 정의
void *thread_function(void *arg) {
    int *num = (int *)arg;
    printf("스레드 번호: %d, 프로세스 PID: %d, 스레드 PID = %ul\n", *num, getpid(), pthread_self());
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    int arg1 = 1, arg2 = 2;

    if (pthread_create(&thread1, NULL, thread_function, &arg1) != 0)  // 첫번째 스레드 생성
        return 1;

    if (pthread_create(&thread2, NULL, thread_function, &arg2) != 0)  // 두번째 스레드 생성
        return 1;
    
    pthread_join(thread1, NULL);          // 스레드가 종료될 때까지 기다림
    pthread_join(thread2, NULL);          // 스레드가 종료될 때까지 기다림

    printf("메인 프로세스 종료, 프로세스 PID: %d\n", getpid());
    return 0;
}