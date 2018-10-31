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


int getSemaphores (sem_t *screen, sem_t * keyboard, int index) {
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

void process (sem_t * screen, sem_t * keyboard, int index) {
    // printf("Hello %d\n", index);
    //   // sem_open both semaphores if necessary
    int count = 0;
    char buf[81];
    buf[80] = '\0';
    do {
        count += getSemaphores(screen, keyboard, index);
        printf("enter < 80 characters or q to quit: \n");
        fgets(buf, 80, stdin);
        printf("%s\n", buf);
        sem_post(screen);
        sem_post(keyboard);
    } while (strncmp(buf, "q\n", 2) != 0);
    count += getSemaphores(screen, keyboard, index);
    printf("This process: %d had %d deadlocks\n", index, count);
    sem_post(screen);
    sem_post(keyboard);
    sem_close(screen);
    sem_close(keyboard);
}

/**
 * Name: Ahmed Radwan
 * Class: CS4540
 */
int main(int argc, char **argv) {
    sem_t *sems = sem_open("/SCREEN", O_CREAT, 0644, 1);
    sem_t *semk = sem_open("/KEYBOARD", O_CREAT, 0644, 1);

    int pids[9];
    int pid;
    int i = 0;
    int j = 0;
    int *statusPtr = NULL;
    
    do {
        if((pid = fork()) == 0) {
            process(sems, semk, i);
        } else {
            pids[i] = pid;
            i++;
        }
    } while( i < 9 && pid > 0);

    if(i == 9) { // stop forking around
        // after all die, use single call or loop based on ids saved above
        for (j = 0; j < 9; j++) {
            waitpid(pids[j], statusPtr, 0);
        }
        // sem_unlink both
        sem_unlink("/SCREEN");
        sem_unlink("/KEYBOARD");
    }


    return 0;
}