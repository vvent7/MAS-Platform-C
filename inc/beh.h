#ifndef BEH_H
#define BEH_H

#include <stdbool.h>
#include "agent.h"

//Beh -> Behaviour
typedef struct{
  Agent *myAgent;
} Beh;

Beh* beh_new(Agent *myAgent);

void beh_free(Beh *b);

void beh_action(Beh *b);

/* Verifies whether the Behaviour is on
 * Returns: true->ON, false->OFF */
bool beh_done(Beh *b);

void beh_on_start(Beh *b);

void beh_on_end(Beh *b);

#endif