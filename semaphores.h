#ifndef SEMAPHORE_STRUCT_H
#define SEMAPHORE_STRUCT_H
#include <semaphore.h>
typedef struct semaphore_struct {
    sem_t *screen;
    sem_t *keyboard;
} semaphore;
#endif
