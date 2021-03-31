#ifndef MAS_H
#define MAS_H

#include <pthread.h>
#include "cstring.h"
#include "queue.h"


/*----------------------------------------Typedefs----------------------------------------*/
typedef enum AgentState AgentState;
typedef struct BehMsg BehMsg;
typedef struct AgentAux AgentAux;
typedef struct AgentOpq AgentOpq;
typedef struct Agent Agent;

typedef enum BehState BehState;
typedef struct BehOpq BehOpq;
typedef struct Beh Beh;

typedef struct Message Message;
/*=======================================================================================*/

/*-----------------------------------------Agent-----------------------------------------*/
enum AgentState{AGENT_NOT_STARTED, AGENT_STARTING, AGENT_RUNNING, AGENT_KILLED};

struct Agent{
/* Visible */
  String *aid, *name;

  /* const methods */
  void (* const run)(Agent *agent);
  AgentState (* const state)(Agent *self);
  void (* const add_beh)(Agent *self, Beh *bh);
  void (* const block_beh)(Agent *self, Beh *bh);
  void (* const unblock_beh)(Agent *self, Beh *bh);
  void (* const do_delete)(Agent *self);
  void (* const send)(Message *msg);
  void (* const receive)(Message *msg);

/* Opaque */
  AgentOpq *opq;
};

void agent_init(Agent *ag, const char *aid, const char *name,
  void (*setup)(Agent *ag), void (*msgHandler)(Message *msg),
  void (*take_down)(Agent *ag));

Agent* agent_new(const char *aid, const char *name,
  void (*setup)(Agent *ag), void (*msgHandler)(Message *msg),
  void (*take_down)(Agent *ag));

void agent_destroy(Agent *ag);
/*=======================================================================================*/

/*---------------------------------------Behavior---------------------------------------*/
enum BehState {BEH_BLOCKED, BEH_ACTIVE};

/* Beh -> Behavior */
struct Beh{
/* Visible */
  Agent *myAgent;

  /* const methods */
  void (* const block)(Beh *self);
  void (* const unblock)(Beh *self);
  BehState (* const state)(Beh *self);
  char (* const done)(Beh *self);

/* Opaque */
  BehOpq *opq;
};

void beh_init(Beh *bh, Agent *myAgent, BehState state,
  char (*done)(Beh *bh), void (*on_start)(Beh *bh),
  void (*action)(Beh *bh), void (*on_end)(Beh *bh));

Beh* beh_new(Agent *myAgent, BehState state,
  char (*done)(Beh *bh), void (*on_start)(Beh *bh),
  void (*action)(Beh *bh), void (*on_end)(Beh *bh));

void beh_destroy(Beh *bh);
/*=======================================================================================*/

/*----------------------------------------Message----------------------------------------*/
typedef struct Message{
  Agent *sender, *recipient;
  void *tag;
  void *data;
  void (*destroy)(struct Message *self);
} Message;

Message* message_new(Agent *sender, Agent *recipient, void *tag,
  void *data, void (*destroy)(Message *msg));
/*=======================================================================================*/

#endif