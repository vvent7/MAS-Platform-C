#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "container.h"
#include "agent.h"
#include "platform.h"
#include "queue.h"

Agent* agent_new(char aid[AGENT_MAX_AID], char name[AGENT_MAX_NAME], char platform[PLAT_MAX_NAME]){
  Agent *ag = (Agent*) malloc(sizeof(Agent));

  strcpy(ag->aid, aid);
  strcpy(ag->name, name);
  strcpy(ag->platform, platform);

  ag->actives = q_new(); ag->blockeds = q_new();

  return ag;
}

void agent_free(Agent *ag){
  free(ag);
  /*
   * It could be possible to free internal components,
   * like Queue *actives, Beh of agents, etc.
   * Implementation should be done later...
   */
}

void* agent_run(Agent *ag, void *x){
  printf("Agent %s: Run\n", ag->name);

  return x;
}

void agent_do_delete(Agent *ag){
  printf("Agent %s: Do Delete\n", ag->name);
}

void agent_setup(Agent *ag){
  printf("Agent %s: Setup\n", ag->name);
}

void agent_take_down(Agent *ag){
  printf("Agent %s: Take Down\n", ag->name);
}

// void send(Message msg);
// Message receive();