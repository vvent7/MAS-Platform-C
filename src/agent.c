#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "cstring.h"
#include "queue.h"
#include "agent.h"
#include "beh.h"

Agent agent_new(const char *aid, const char *name, const char *platform,
  void (*setup)(struct Agent *ag), void (*take_down)(struct Agent *ag)){
  _agent_opaque *opaque = (_agent_opaque*) malloc(sizeof(_agent_opaque));
  opaque->state = AGENT_NOT_STARTED;
  opaque->busyAgentLock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
  opaque->addActiveBehCond = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
  pthread_mutex_init(opaque->busyAgentLock, NULL);
  pthread_cond_init(opaque->addActiveBehCond, NULL);

  return ((Agent){
    .aid = string_new_copy_cfg(aid, CSTRING_TP_ARR, CSTRING_CAPMAN_EQUAL_SIZE, 1),
    .name = string_new_copy_cfg(name, CSTRING_TP_ARR, CSTRING_CAPMAN_EQUAL_SIZE, 1),
    .platform = string_new_copy_cfg(platform, CSTRING_TP_ARR, CSTRING_CAPMAN_EQUAL_SIZE, 1),
    .actives = queue_new(), .blockeds = queue_new(), .run = agent_run,
    .state = agent_running, .add_beh = agent_add_beh,
    .blocked = agent_blocked, .unblocked = agent_unblocked,
    .do_delete = agent_do_delete, .setup=setup, .take_down=take_down,
    .opaque = opaque
  });
}

Agent* agent_new_ptr(const char *aid, const char *name, const char *platform,
  void (*setup)(struct Agent *ag), void (*take_down)(struct Agent *ag)){
  Agent aux = agent_new(aid, name, platform, setup, take_down),
    *ag = (Agent*) malloc(sizeof(Agent));

  memcpy(ag, &aux, sizeof(Agent));

  return ag;
}

void* agent_run(void *agent){
	Agent *ag = (Agent*) agent;
	if(ag){
    _agent_opaque *opaque = (_agent_opaque*) ag->opaque;
    Queue *actives = ag->actives;
    Beh *bh;

    pthread_mutex_lock(opaque->busyAgentLock);
    opaque->state = AGENT_STARTING;
    ag->setup(ag);
    opaque->state = AGENT_RUNNING;
    pthread_mutex_unlock(opaque->busyAgentLock);

		while(opaque->state==AGENT_RUNNING){
      pthread_mutex_lock(opaque->busyAgentLock);

      while((bh = (Beh*) actives->pop(actives, NULL))==NULL){
        pthread_cond_wait(opaque->addActiveBehCond, opaque->busyAgentLock);
        if(opaque->state==AGENT_KILLED) break;
      }
      
      if(bh){
        bh->action(bh);
        if(!bh->done(bh)) actives->push(actives, bh);
        else bh->on_end(bh);
      }

      pthread_mutex_unlock(opaque->busyAgentLock);
		}
		ag->take_down(ag);
	}

  pthread_exit(NULL);
}

AgentState agent_running(Agent *ag){
  return ag ? ((_agent_opaque*)ag->opaque)->state : AGENT_KILLED;
}

void agent_add_beh(Agent *ag, Beh *bh){
  if(ag && bh){
    _agent_opaque *opaque = (_agent_opaque *) ag->opaque;
    pthread_mutex_lock(opaque->busyAgentLock);
    bh->on_start(bh);
    ag->actives->push(ag->actives, bh);
    pthread_cond_signal(opaque->addActiveBehCond);
    pthread_mutex_unlock(opaque->busyAgentLock);
  }
}

void agent_blocked(Agent *ag, Beh *bh){
  if(ag && bh){
    _agent_opaque *opaque = (_agent_opaque *) ag->opaque;
    pthread_mutex_lock(opaque->busyAgentLock);
    if(ag->actives->delete(ag->actives, bh, NULL, NULL))
      ag->blockeds->push(ag->blockeds, bh);
    pthread_mutex_unlock(opaque->busyAgentLock);
  }
}

void agent_unblocked(Agent *ag, Beh *bh){
  if(ag && bh){
    _agent_opaque *opaque = (_agent_opaque *) ag->opaque;
    pthread_mutex_lock(opaque->busyAgentLock);
    if(ag->blockeds->delete(ag->blockeds, bh, NULL, NULL)){
      ag->actives->push(ag->actives, bh);
      pthread_cond_signal(opaque->addActiveBehCond);
    }
    pthread_mutex_unlock(opaque->busyAgentLock);
  }
}

void agent_do_delete(Agent *ag){
  if(ag){
    _agent_opaque *opaque = (_agent_opaque*) ag->opaque;
    pthread_mutex_lock(opaque->busyAgentLock);
    opaque->state = AGENT_KILLED;
    pthread_cond_signal(opaque->addActiveBehCond);
    pthread_mutex_unlock(opaque->busyAgentLock);
  }
}

void agent_opaque_free(Agent *ag){
  _agent_opaque *opaque = (_agent_opaque*) ag->opaque;
  pthread_mutex_destroy(opaque->busyAgentLock); free(opaque->busyAgentLock);
  pthread_cond_destroy(opaque->addActiveBehCond); free(opaque->addActiveBehCond);
  free(opaque);
}