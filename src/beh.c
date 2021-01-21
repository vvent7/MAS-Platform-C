#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "beh.h"
#include "agent.h"

Beh* beh_new(Agent *myAgent){
  Beh *b = (Beh*) malloc(sizeof(Beh));

  b->myAgent = myAgent;

  return b;
}

void beh_free(Beh *b){
  free(b);
}

void beh_action(Beh *b){
  printf("Beh of Agent %s: Action\n", b->myAgent->name);
}

bool beh_done(Beh *b){
  printf("Beh of Agent %s: Done\n", b->myAgent->name);

  return true;
}

void beh_on_start(Beh *b){
  printf("Beh of Agent %s: On Start\n", b->myAgent->name);
}

void beh_on_end(Beh *b){
  printf("Beh of Agent %s: On End\n", b->myAgent->name);
}