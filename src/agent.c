#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "agent.h"
#include "beh.h"
#include "queue.h"
#include "cstring.h"

Agent* agent_new(String *aid, String *name, String *platform){
  Agent *ag = (Agent*) malloc(sizeof(Agent));
  ag->aid = aid; ag->name = name; ag->platform = platform;
  ag->actives = queue_new(); ag->blockeds = queue_new();
  ag->start = 0;

  ag->run = agent_run;
  ag->add_beh = agent_add_beh;
  ag->blocked = agent_blocked;
  ag->unblocked = agent_unblocked;
  ag->do_delete = agent_do_delete;
  ag->setup = agent_setup;
  ag->take_down = agent_take_down;

  return ag;
}

void agent_free(Agent *ag){
  string_ptr_free(ag->aid); string_ptr_free(ag->name);
  ag->actives->free(ag->actives, beh_freeData);
  ag->blockeds->free(ag->blockeds, beh_freeData);
  free(ag);
}

void* agent_run(void *x){
	Agent *ag = (Agent*) x;
	if(ag){
		ag->setup(ag);
    Queue *actives = ag->actives, *blockeds = ag->blockeds;
		while(ag->start){
      Beh *bh = (Beh*) actives->pop(actives, NULL);

      if(bh){
        bh->action(bh);
        if(bh->done(bh)) bh->free(bh);
        else actives->push(actives, bh);
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
  ag->start = 0;
}

void agent_setup(Agent *ag){
  ag->start = 1;
}

void agent_take_down(Agent *ag){
  agent_free(ag);
}

// void send(Message msg);
// Message receive();