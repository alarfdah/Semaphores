#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>
#include "semaphores.h"
#include "args.h"

int get_semaphores(sem_t *screen, sem_t * keyboard, int index) {
    sem_t *first;
    sem_t *second;

    struct timespec time;
    int timedwaitResult = 0;
    int count = 0;
    int done = 0;
    //   returns count of how many times recovered from deadlock

    if (index % 2 == 1) {
        first = screen;
        second = keyboard;
    } else {
        first = keyboard;
        second = screen;
    }

    do {
        clock_gettime(CLOCK_REALTIME, &time);
        time.tv_sec += 1;

        sem_wait(first);
        sleep(1);
        timedwaitResult = sem_timedwait(second, &time);
        
        if (timedwaitResult == -1) {
            sem_post(first);
            sleep(rand() % 5);
            count++;
        } else {
            done = 1;
        }
        
    } while(!done);
    
    return count;
}

void *pthread_process(void *arguments) {
    struct args_struct *args = arguments;
    int count = 0;
    char buf[81];
    buf[80] = '\0';
    do {
        count += get_semaphores(args->sem->screen, args->sem->keyboard, args->index);
        printf("enter < 80 characters or q to quit: \n");
        fgets(buf, 80, stdin);
        printf("%s\n", buf);
        sem_post(args->sem->screen);
        sem_post(args->sem->keyboard);
    } while (strncmp(buf, "q\n", 2) != 0);
    count += get_semaphores(args->sem->screen, args->sem->keyboard, args->index);
    printf("This process: %d had %d deadlocks\n", args->index, count);
    sem_post(args->sem->screen);
    sem_post(args->sem->keyboard);
    sem_close(args->sem->screen);
    sem_close(args->sem->keyboard);
    pthread_exit(NULL);
}

/**
 * Name: Ahmed Radwan
 * Class: CS4540
 */
int main(int argc, char **argv) {
    pthread_t pthreads[9];
    // pthread_t pthread;

    int i = 0;

    semaphore *sem = malloc(sizeof(semaphore));
    sem->screen = malloc(sizeof(sem_t));
    sem->keyboard = malloc(sizeof(sem_t));
    sem_init(sem->screen, 0, 1);
    sem_init(sem->keyboard, 0, 1);


    for (i = 0; i < 9; i++) {
        struct args_struct *args = malloc(sizeof(struct args_struct));
        args->index = i;
        args->sem = sem;
        pthread_create(&(pthreads[i]), NULL, pthread_process, (void *) args);
    }

    for (i = 0; i < 9; i++) {
        pthread_join(pthreads[i], NULL);
    }
    // sem_destroy both
    sem_destroy(sem->screen);
    sem_destroy(sem->keyboard);

    return 0;
}