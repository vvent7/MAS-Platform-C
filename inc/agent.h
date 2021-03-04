#ifndef AGENT_H
#define AGENT_H

#include "cstring.h"
#include "queue.h"
#include "beh.h"

typedef struct Agent{
/* public attributes */
  String *aid, *name, *platform;
  Queue *actives, *blockeds;
  unsigned short running;

/* private methods */
  void* (*run)(void *arg);
  void (*add_beh)(struct Agent *this, Beh *bh);
  void (*blocked)(struct Agent *this, Beh *bh);
  void (*unblocked)(struct Agent *this, Beh *bh);
  void (*do_delete)(struct Agent *this);

/* public methods */
  void (*setup)(struct Agent *this);
  void (*take_down)(struct Agent *this);
  /* void send(struct Agent *this, Message msg); 
   * Message receive(struct Agent *this) */
} Agent;

Agent* agent_new(String *aid, String *name, String *platform,
  void (*setup)(struct Agent *ag),
  void (*take_down)(struct Agent *ag));

void* agent_run(void*);

void agent_add_beh(Agent *ag, Beh *bh);

void agent_blocked(Agent *ag, Beh *bh);

void agent_unblocked(Agent *ag, Beh *bh);

void agent_do_delete(Agent *ag);

#endif