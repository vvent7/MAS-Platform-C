#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "container.h"
#include "queue.h"

Container* container_new(char processId[CONT_MAX_PROC_ID], char name[CONT_MAX_NAME]){
  Container *c = (Container *) malloc(sizeof(Container));

  strcpy(c->processId, processId);
  strcpy(c->name, name);
  c->agents = q_new();

  return c;
}

void container_free(Container *cont){
  free(cont);
  /*
   * It could be possible to free internal components,
   * like Queue *agents, Beh of agents, etc.
   * Implementation should be done later...
   */
}

void* container_run(Container *cont, void* x){
  printf("Container %s: Run\n", cont->name);
  return x;
}