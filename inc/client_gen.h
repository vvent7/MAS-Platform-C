#ifndef AGENT_CLIENT_GEN_H
#define AGENT_CLIENT_GEN_H

#include "mas.h"
#include "store.h"

typedef struct{
  Agent super;
  AgentStore *agStore;
}AgentClientGen;

AgentClientGen* agent_client_gen_new(const char *aid, const char *name, AgentStore *agStore);

#endif