#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "agent.h"
#include "beh.h"
#include "queue.h"
#include "cstring.h"

Agent* agent_new(String *aid, String *name, String *platform,
  void (*setup)(struct Agent *ag),
  void (*take_down)(struct Agent *ag)){
  Agent *ag = (Agent*) malloc(sizeof(Agent));
  ag->aid = aid; ag->name = name; ag->platform = platform;
  ag->actives = queue_new(); ag->blockeds = queue_new();
  ag->running = 0;

  ag->run = agent_run;
  ag->add_beh = agent_add_beh;
  ag->blocked = agent_blocked;
  ag->unblocked = agent_unblocked;
  ag->do_delete = agent_do_delete;
  ag->setup = setup;
  ag->take_down = take_down;

  return ag;
}

void* agent_run(void *x){
	Agent *ag = (Agent*) x;
	if(ag){
    ag->running = 1;
		ag->setup(ag);
    Queue *actives = ag->actives, *blockeds = ag->blockeds;
		while(ag->running){
      Beh *bh = (Beh*) actives->pop(actives, NULL);

      if(bh){
        bh->action(bh);
        if(!bh->done(bh))
          actives->push(actives, bh);
      }
		}
		ag->take_down(ag);
	}

  pthread_exit(NULL);
}

void agent_add_beh(Agent *ag, Beh *bh){
  ag->actives->push(ag->actives, bh);
}

void agent_blocked(Agent *ag, Beh *bh){
  /* To implement... */
}

void agent_unblocked(Agent *ag, Beh *bh){
  /* To implement... */
}

void agent_do_delete(Agent *ag){
  ag->running = 0;
}