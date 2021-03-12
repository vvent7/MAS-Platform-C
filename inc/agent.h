#ifndef AGENT_H
#define AGENT_H

#include <pthread.h>
#include "cstring.h"
#include "queue.h"
#include "beh.h"


typedef enum{AGENT_NOT_STARTED, AGENT_STARTING, AGENT_RUNNING, AGENT_KILLED}AgentState;
typedef struct{
  AgentState state;
  pthread_mutex_t *busyAgentLock;
  pthread_cond_t *addActiveBehCond;
} _agent_opaque;

typedef struct Agent{
/* Visible */
  String *aid, *name, *platform;
  Queue *actives, *blockeds;

  /* const methods */
  void* (* const run)(void *agent);
  AgentState (* const state)(struct Agent *this);
  void (* const add_beh)(struct Agent *this, Beh *bh);
  void (* const blocked)(struct Agent *this, Beh *bh);
  void (* const unblocked)(struct Agent *this, Beh *bh);
  void (* const do_delete)(struct Agent *this);
  void (* const agent_opaque_free)(struct Agent *this);

  /* changeable methods */
  void (*setup)(struct Agent *this);
  void (*take_down)(struct Agent *this);

/* Opaque */
  void *opaque;
} Agent;

Agent agent_new(const char *aid, const char *name, const char *platform,
  void (*setup)(struct Agent *ag), void (*take_down)(struct Agent *ag));

Agent* agent_new_ptr(const char *aid, const char *name, const char *platform,
  void (*setup)(struct Agent *ag), void (*take_down)(struct Agent *ag));

void* agent_run(void* agent);

AgentState agent_running(Agent *ag);

void agent_add_beh(Agent *ag, Beh *bh);

void agent_blocked(Agent *ag, Beh *bh);

void agent_unblocked(Agent *ag, Beh *bh);

void agent_do_delete(Agent *ag);

void agent_opaque_free(Agent *ag);

#endif