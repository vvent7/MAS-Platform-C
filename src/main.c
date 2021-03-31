#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "client_gen.h"

int main(){
  AgentStore *agStore = agent_store_new("1", "AgentStore1");
  AgentClientGen *agGen = agent_client_gen_new("2", "AgentClientGen1", agStore);
  srand(time(NULL));

  agStore->super.run((Agent*) agStore);
  agGen->super.run((Agent*) agGen);

  pthread_exit(NULL);
}

