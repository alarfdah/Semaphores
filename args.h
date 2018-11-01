#ifndef ARGS_STRUCT_H
#define ARGS_STRUCT_H

#include "semaphores.h"
struct args_struct {
    int index;
    semaphore *sem;
};
#endif