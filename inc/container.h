#ifndef CONTAINER_H
#define CONTAINER_H

#include "queue.h"

#define CONT_MAX_PROC_ID 101
#define CONT_MAX_NAME 101

typedef struct{
  char processId[CONT_MAX_PROC_ID], name[CONT_MAX_NAME];
  Queue *agents;
} Container;

Container* container_new(char processId[CONT_MAX_PROC_ID], char name[CONT_MAX_NAME]);

void container_free(Container *cont);

void* container_run(Container *cont, void* x);

#endif