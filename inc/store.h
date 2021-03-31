#ifndef AGENT_STORE_H
#define AGENT_STORE_H

#include "mas.h"

typedef struct AgentStore AgentStore;

struct AgentStore{
  Agent super;
  Queue *clients;
};

typedef enum StoreMsgTags{MSG_CLIENT_ARRIVED}StoreMsgTags;

AgentStore* agent_store_new(const char *aid, const char *name);

Message* message_store_new(Agent *sender, Agent *recipient, StoreMsgTags tag, void *data);

#endif