#include <stdio.h>
#include <stdlib.h>
#include "beh.h"
#include "agent.h"

Beh* beh_new(Agent *myAgent, void (*action)(struct Beh *bh),
  unsigned short (*done)(struct Beh *bh),
  void (*on_start)(struct Beh *bh),
  void (*on_end)(struct Beh *bh)){
  Beh *bh = (Beh*) malloc(sizeof(Beh));
  bh->myAgent = myAgent;

  bh->blocked = blocked;
  bh->unblocked = unblocked;

  bh->action = action;
  bh->done = done;
  bh->on_start = on_start;
  bh->on_end = on_end;

  return bh;
}

void blocked(Beh *bh){
  bh->myAgent->blocked(bh->myAgent, bh);
}

void unblocked(Beh *bh){
  bh->myAgent->unblocked(bh->myAgent, bh);
}