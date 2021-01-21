#ifndef AGENT_H
#define AGENT_H

#include "platform.h"
#include "queue.h"

#define AGENT_MAX_AID 100
#define AGENT_MAX_NAME 100

typedef struct{
  char aid[AGENT_MAX_AID], name[AGENT_MAX_NAME], platform[PLAT_MAX_NAME];
  Queue *actives, *blockeds;
} Agent;

Agent* agent_new(char aid[AGENT_MAX_AID], char name[AGENT_MAX_NAME], char platform[PLAT_MAX_NAME]);

void agent_free(Agent *ag);

void* agent_run(Agent *ag, void*);

void agent_do_delete(Agent *ag);

void agent_setup(Agent *ag);

void agent_take_down(Agent *ag);

// void send(Message msg);
// Message receive();

#endif