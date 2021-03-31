#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "cstring.h"
#include "cvector.h"
#include "mas.h"

/*--------------------------------------Definitions--------------------------------------*/
struct AgentOpq{
  AgentState state;
  BehMsg *bhMsg;

  Queue *activeBehs, *blockedBehs;
  pthread_mutex_t *busyAgentLock, *busyMessageLock;
  pthread_cond_t *addActiveBehCond;

  void (*setup)(Agent *self);
  void (*take_down)(Agent *self);
};

struct BehOpq{
  BehState state;
  void (*on_start)(Beh *self);
  void (*action)(Beh *self);
  void (*on_end)(Beh *self);
};

struct BehMsg{
  Beh super;
  Queue *messages;
  void (*msgHandler)(Message *msg);
};
/*=======================================================================================*/

/*--------------------------------------Prototypes--------------------------------------*/
static void agent_free_queue_beh(void *data);
static void agent_opq_setup(Agent *ag);
static void* agent_run_thread(void* agent);
static void agent_run(Agent *ag);
static AgentState agent_state(Agent *ag);
static void agent_add_beh(Agent *ag, Beh *bh);
static void agent_block_beh(Agent *ag, Beh *bh);
static void agent_unblock_beh(Agent *ag, Beh *bh);
static void agent_do_delete(Agent *ag);
static void agent_send(Message *msg);
static void agent_receive(Message *msg);

static void beh_block(Beh *bh);
static void beh_unblock(Beh *bh);
static BehState beh_state(Beh *bh);

static BehMsg* beh_msg_new(Agent *myAgent, BehState state, void (*msgHandler)(Message *msg));
static char beh_msg_done(Beh *bh);
static void beh_msg_on_start(Beh *bh);
static void beh_msg_action(Beh *bh);
static void beh_msg_on_end(Beh *bh);
/*======================================================================================*/

/*-----------------------------------------Agent-----------------------------------------*/
void agent_init(Agent *ag, const char *aid, const char *name,
  void (*setup)(Agent *ag), void (*msgHandler)(Message *msg),
  void (*take_down)(Agent *ag)){
  AgentOpq *agOpq = (AgentOpq*) malloc(sizeof(AgentOpq));
  *agOpq = ((AgentOpq){
    .state = AGENT_NOT_STARTED,
    .bhMsg = beh_msg_new(ag, BEH_ACTIVE, msgHandler),
    .activeBehs = NULL, .blockedBehs = NULL,
    .busyAgentLock = NULL, .busyMessageLock = NULL,
    .addActiveBehCond = NULL,
    .setup = setup, .take_down = take_down,
  });

  Agent aux = ((Agent){
    .aid = string_new_copy_cfg(aid, CSTRING_TP_ARR, CSTR_CAPMAN_EQ_SZ, 1),
    .name = string_new_copy_cfg(name, CSTRING_TP_ARR, CSTR_CAPMAN_EQ_SZ, 1),
    .run = agent_run, .state = agent_state, .add_beh = agent_add_beh,
    .block_beh = agent_block_beh, .unblock_beh = agent_unblock_beh,
    .do_delete = agent_do_delete, .send = agent_send, .receive = agent_receive,
    .opq = agOpq
  });

  memcpy(ag, &aux, sizeof(Agent));
}

Agent* agent_new(const char *aid, const char *name,
  void (*setup)(Agent *ag), void (*msgHandler)(Message *msg),
  void (*take_down)(Agent *ag)){
  Agent *ag = (Agent*) malloc(sizeof(Agent));
  agent_init(ag, aid, name, setup, msgHandler, take_down);
  return ag;
}

static void agent_free_queue_beh(void *data){
  Beh *bh = (Beh*) data;
  if(bh && bh->opq && bh->opq->on_end)
    bh->opq->on_end(bh);
}

void agent_destroy(Agent *ag){
  AgentOpq *agOpq = ag->opq;

  string_free(ag->aid); string_free(ag->name);

  agOpq->activeBehs->free(agOpq->activeBehs, agent_free_queue_beh);
  agOpq->blockedBehs->free(agOpq->blockedBehs, agent_free_queue_beh);

  pthread_mutex_destroy(agOpq->busyAgentLock); free(agOpq->busyAgentLock);
  pthread_mutex_destroy(agOpq->busyMessageLock); free(agOpq->busyMessageLock);
  pthread_cond_destroy(agOpq->addActiveBehCond); free(agOpq->addActiveBehCond);

  free(agOpq);
}

static void agent_opq_setup(Agent *ag){
  if(ag && ag->opq){
    AgentOpq *agOpq = ag->opq;
    pthread_mutexattr_t mutexAttr;

    agOpq->activeBehs = queue_new();
    agOpq->blockedBehs = queue_new();

    agOpq->busyAgentLock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    agOpq->busyMessageLock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    agOpq->addActiveBehCond = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
    
    pthread_mutexattr_init(&mutexAttr);
    pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(agOpq->busyAgentLock, &mutexAttr);
    pthread_mutex_init(agOpq->busyMessageLock, &mutexAttr);
    pthread_mutexattr_destroy(&mutexAttr);

    pthread_cond_init(agOpq->addActiveBehCond, NULL);
    
    ag->add_beh(ag, (Beh*) agOpq->bhMsg);
  }
}

static void* agent_run_thread(void *agent){
	Agent *ag = (Agent*) agent;
	if(ag){
    AgentOpq *agOpq = ag->opq;
    agent_opq_setup(ag);

    pthread_mutex_lock(agOpq->busyAgentLock);
    agOpq->state = AGENT_STARTING;
    if(agOpq->setup) agOpq->setup(ag);
    agOpq->state = AGENT_RUNNING;
    pthread_mutex_unlock(agOpq->busyAgentLock);

    pthread_mutex_lock(agOpq->busyAgentLock);
		while(agOpq->state==AGENT_RUNNING){
      Queue *activeBehs = agOpq->activeBehs,
        *blockedBehs = agOpq->blockedBehs;
      
      while(agOpq->state!=AGENT_KILLED && activeBehs->empty(activeBehs))
        pthread_cond_wait(agOpq->addActiveBehCond, agOpq->busyAgentLock);
      
      if(agOpq->state!=AGENT_KILLED){
        Beh *bh = (Beh*) activeBehs->front(activeBehs);
        if(bh){
          pthread_mutex_unlock(agOpq->busyAgentLock);
          bh->opq->action(bh);
          pthread_mutex_lock(agOpq->busyAgentLock);
          
          if(bh->done(bh)){
            if(bh->state(bh)==BEH_ACTIVE)
              activeBehs->delete(activeBehs, bh, NULL, NULL);
            else
              blockedBehs->delete(blockedBehs, bh, NULL, NULL);
            
            if(bh->opq->on_end) bh->opq->on_end(bh);
          }
          else if(activeBehs->front(activeBehs)==bh)
            activeBehs->push(activeBehs, activeBehs->pop(activeBehs, NULL));
        }
        else activeBehs->pop(activeBehs, NULL);
      }

      pthread_mutex_unlock(agOpq->busyAgentLock);
      pthread_mutex_lock(agOpq->busyAgentLock);
		}
    pthread_mutex_unlock(agOpq->busyAgentLock);
		agOpq->take_down(ag);
	}
  pthread_exit(NULL);
}

static void agent_run(Agent *ag){
  if(ag && ag->run){
    pthread_t thread;
    pthread_create(&thread, NULL, agent_run_thread, ag);
  }
}

static AgentState agent_state(Agent *ag){
  return ag && ag->opq ? ag->opq->state : AGENT_KILLED;
}

static void agent_add_beh(Agent *ag, Beh *bh){
  if(ag && bh && bh->opq){
    AgentOpq *agOpq = ag->opq;
    pthread_mutex_lock(agOpq->busyAgentLock);

    if(bh->opq->on_start) bh->opq->on_start(bh);

    switch(bh->state(bh)){
      case BEH_ACTIVE:
        agOpq->activeBehs->push(agOpq->activeBehs, bh);
        pthread_cond_signal(agOpq->addActiveBehCond);
        break;
      case BEH_BLOCKED:
        agOpq->blockedBehs->push(agOpq->blockedBehs, bh);
        break;
      default:
        bh->opq->state = BEH_BLOCKED;
        agOpq->blockedBehs->push(agOpq->blockedBehs, bh);
    }

    pthread_mutex_unlock(agOpq->busyAgentLock);
  }
}

static void agent_block_beh(Agent *ag, Beh *bh){
  if(ag && bh && bh->state(bh)==BEH_ACTIVE){
    AgentOpq *agOpq = ag->opq;

    pthread_mutex_lock(agOpq->busyAgentLock);

    if(agOpq->activeBehs->delete(agOpq->activeBehs, bh, NULL, NULL)){
      agOpq->blockedBehs->push(agOpq->blockedBehs, bh);
      bh->opq->state = BEH_BLOCKED;
    }

    pthread_mutex_unlock(agOpq->busyAgentLock);
  }
}

static void agent_unblock_beh(Agent *ag, Beh *bh){
  if(ag && bh && bh->state(bh)==BEH_BLOCKED){
    AgentOpq *agOpq = ag->opq;

    pthread_mutex_lock(agOpq->busyAgentLock);

    if(agOpq->blockedBehs->delete(agOpq->blockedBehs, bh, NULL, NULL)){
      agOpq->activeBehs->push(agOpq->activeBehs, bh);
      bh->opq->state = BEH_ACTIVE;
      pthread_cond_signal(agOpq->addActiveBehCond);
    }

    pthread_mutex_unlock(agOpq->busyAgentLock);
  }
}

static void agent_do_delete(Agent *ag){
  if(ag){
    AgentOpq *agOpq = ag->opq;
    pthread_mutex_lock(agOpq->busyAgentLock);
    agOpq->state = AGENT_KILLED;
    pthread_cond_signal(agOpq->addActiveBehCond);
    pthread_mutex_unlock(agOpq->busyAgentLock);
  }
}

static void agent_send(Message *msg){
  if(msg && msg->recipient && msg->recipient->receive)
    msg->recipient->receive(msg);
}

static void agent_receive(Message *msg){
  if(msg && msg->recipient){
    AgentOpq *agOpq = msg->recipient->opq;
    if(agOpq && agOpq->state==AGENT_RUNNING && agOpq->bhMsg){
      Beh *bhMsg = (Beh*) agOpq->bhMsg;
      Queue *messages = agOpq->bhMsg->messages;

      if(messages==NULL) return;

      pthread_mutex_lock(agOpq->busyMessageLock);
      messages->push(messages, msg);
      bhMsg->unblock(bhMsg);
      pthread_mutex_unlock(agOpq->busyMessageLock);
    }
  }
}
/*=======================================================================================*/

/*---------------------------------------Behavior---------------------------------------*/
void beh_init(Beh *bh, Agent *myAgent, BehState state,
  char (*done)(Beh *bh), void (*on_start)(Beh *bh),
  void (*action)(Beh *bh), void (*on_end)(Beh *bh)){

  BehOpq *bhOpq = (BehOpq*) malloc(sizeof(BehOpq));
  *bhOpq = ((BehOpq){
    .state = state, .on_start = on_start,
    .action = action, .on_end = on_end
  });

  Beh aux = ((Beh){
    .block = beh_block, .unblock = beh_unblock, .state = beh_state,
    .myAgent = myAgent, .done = done, .opq = bhOpq
  });
  
  memcpy(bh, &aux, sizeof(Beh));
}

Beh* beh_new(Agent *myAgent, BehState state,
  char (*done)(Beh *bh), void (*on_start)(Beh *bh),
  void (*action)(Beh *bh), void (*on_end)(Beh *bh)){
  if(myAgent && action && done){
    Beh *bh = (Beh*) malloc(sizeof(Beh));
    beh_init(bh, myAgent, state, done, on_start, action, on_end);
    return bh;
  }
  return NULL;
}

void beh_destroy(Beh *bh){
  if(bh) free(bh->opq);
}

static void beh_block(Beh *bh){
  if(bh) bh->myAgent->block_beh(bh->myAgent, bh);
}

static void beh_unblock(Beh *bh){
  if(bh) bh->myAgent->unblock_beh(bh->myAgent, bh);
}

static BehState beh_state(Beh *bh){
  return bh ? bh->opq->state : BEH_BLOCKED;
}

/*=======================================================================================*/

/*----------------------------------------Message----------------------------------------*/
Message* message_new(Agent *sender, Agent *recipient, void *tag,
  void *data, void (*destroy)(Message *msg)){
  if(sender && recipient){
    Message *msg = (Message*) malloc(sizeof(Message));
    *msg = ((Message){
      .sender = sender, .recipient = recipient,
      .tag = tag, .data = data, .destroy = destroy 
    });
    return msg;
  }
  return NULL;
}
/*=======================================================================================*/

/*------------------------------------Behavior Message------------------------------------*/
static BehMsg* beh_msg_new(Agent *myAgent, BehState state, void (*msgHandler)(Message *msg)){
  if(myAgent && msgHandler){
    BehMsg *bhMsg = (BehMsg*) malloc(sizeof(BehMsg));
    beh_init(&(bhMsg->super), myAgent, state,
      beh_msg_done, beh_msg_on_start, beh_msg_action, beh_msg_on_end);
    bhMsg->messages = NULL;
    bhMsg->msgHandler = msgHandler;
    return bhMsg;
  }
  return NULL;
}

static char beh_msg_done(Beh *bh){
  return bh && bh->myAgent
    ? bh->myAgent->state(bh->myAgent) == AGENT_KILLED
    : 1;
}

static void beh_msg_on_start(Beh *bh){
  if(bh){
    BehMsg *bhMsg = (BehMsg*) bh;
    bhMsg->messages = queue_new();
  }
}

static void beh_msg_action(Beh *bh){
  if(bh && bh->myAgent){
    AgentOpq *agOpq = bh->myAgent->opq;
    BehMsg *bhMsg = (BehMsg*) bh;
    Queue *messages = bhMsg->messages;
    pthread_mutex_lock(agOpq->busyMessageLock);
    while(!messages->empty(messages)){
      Message *msg = (Message*) messages->pop(messages, NULL);
      pthread_mutex_unlock(agOpq->busyMessageLock);
      bhMsg->msgHandler(msg);
      if(msg->destroy) msg->destroy(msg);
      pthread_mutex_lock(agOpq->busyMessageLock);
    }
    bh->myAgent->block_beh(bh->myAgent, bh);
    pthread_mutex_unlock(agOpq->busyMessageLock);
  }
}

static void beh_msg_on_end(Beh *bh){
  beh_destroy(bh);
  free(((BehMsg*) bh));
}
/*=======================================================================================*/