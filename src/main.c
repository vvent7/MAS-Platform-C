 /*
    System Entry Point
    Author: Fernando Vítor Ventilari Neder
    Date: 20 January 2021
    (c) All rights reserved
 */

#include <stdio.h>
#include "platform.h"
#include "queue.h"
#include "container.h"
#include "agent.h"
#include "beh.h"

int main(int argc, char const *argv[]){
  
  Container *main = container_new("main_process_123", "Container v1");
  Platform *plat = platform_new("Platform v1", "Endpoint v1", main);
  Agent *agents[]={
    agent_new("ag_1", "James", plat->name),
    agent_new("ag_2", "Bond", plat->name),
    agent_new("ag_3", "Pong", plat->name)
  };
  Beh *bs[3];

  int i;
  for(i=0;i<3;i++){
    bs[i] = beh_new(agents[i]);
    q_push(main->agents, agents[i]);
  }

  //Chamando funções com os valores criados:

  //Container *main
  printf("Container:\n");
  container_run(main, NULL);

  //Agents *agents:
  printf("\n\nAgents:");

  for(i=0;i<3;i++){
    printf("\n");
    agent_run(agents[i], NULL);
    agent_do_delete(agents[i]);
    agent_setup(agents[i]);
    agent_take_down(agents[i]);
  }

  //Beh *bs:
  printf("\n\nBehaviours:");
  for(i=0;i<3;i++){
    printf("\n");
    beh_action(bs[i]);
    beh_done(bs[i]);
    beh_on_start(bs[i]);
    beh_on_end(bs[i]);
  }

  return 0;
}

