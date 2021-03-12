#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "beh.h"
#include "agent.h"

Beh beh_new(Agent *myAgent, void (*action)(struct Beh *bh),
  char (*done)(struct Beh *bh), void (*on_start)(struct Beh *bh),
  void (*on_end)(struct Beh *bh)){
  return (Beh){
    .blocked = beh_blocked, .unblocked = beh_unblocked,
    .myAgent = myAgent, .action = action, .done = done,
    .on_start = on_start, .on_end = on_end
  };
}

Beh* beh_new_ptr(Agent *myAgent, void (*action)(struct Beh *bh),
  char (*done)(struct Beh *bh), void (*on_start)(struct Beh *bh),
  void (*on_end)(struct Beh *bh)){
  Beh aux = beh_new(myAgent, action, done, on_start, on_end),
    *bh = (Beh*) malloc(sizeof(Beh));

  memcpy(bh, &aux, sizeof(Beh));

  return bh;
}

void beh_blocked(Beh *bh){
  bh->myAgent->blocked(bh->myAgent, bh);
}

void beh_unblocked(Beh *bh){
  bh->myAgent->unblocked(bh->myAgent, bh);
}