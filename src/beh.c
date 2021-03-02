#include <stdio.h>
#include <stdlib.h>
#include "beh.h"
#include "agent.h"

Beh* beh_new(Agent *myAgent){
  Beh *bh = (Beh*) malloc(sizeof(Beh));
  bh->myAgent = myAgent; bh->dn = 0;

  bh->free = beh_free;
  bh->action = beh_action;
  bh->done = beh_done;
  bh->on_start = beh_on_start;
  bh->on_end = beh_on_end;

  return bh;
}

void beh_freeData(void *x){
  Beh *bh = (Beh*) x;
  bh->free(bh);
}

void beh_free(Beh *bh){
  bh->on_end(bh);
  free(bh);
}

void blocked(Beh *bh){
  bh->myAgent->blocked(bh->myAgent, bh);
}

void unblocked(Beh *bh){
  bh->myAgent->unblocked(bh->myAgent, bh);
}

void beh_action(Beh *bh){
  printf("Beh of Agent %s: Action\n", *(bh->myAgent->name));
}

unsigned short beh_done(Beh *bh){
  return bh->dn ? 1 : 0;
}

void beh_on_start(Beh *bh){
  printf("Beh of Agent %s: On Start\n", *(bh->myAgent->name));
}

void beh_on_end(Beh *bh){
  printf("Beh of Agent %s: On End\n", *(bh->myAgent->name));
}