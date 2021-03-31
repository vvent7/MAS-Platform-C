#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mas.h"
#include "client.h"
#include "store.h"
#include "misc.h"

#define SERVICE_TIME_MIN 500
#define SERVICE_TIME_RANGE 3000

/*--------------------------------------Prototypes--------------------------------------*/
static void agent_store_setup(Agent *ag);
static void agent_store_msg_handler(Message *msg);
static void agent_store_take_down(Agent *ag);

static void beh_store_action(Beh *bh);
static char beh_store_done(Beh *bh);
static void beh_store_on_end(Beh *bh);

static void message_store_destroy(Message *msg);
/*======================================================================================*/

/*-----------------------------------------Agent-----------------------------------------*/
AgentStore* agent_store_new(const char *aid, const char *name){
  AgentStore *agStore = (AgentStore*) malloc(sizeof(AgentStore));
  agent_init(&(agStore->super), aid, name, agent_store_setup,
    agent_store_msg_handler, agent_store_take_down);
  agStore->clients = NULL;

  return agStore;
}

static void agent_store_setup(Agent *ag){
  if(ag){
    AgentStore *agStore = (AgentStore*) ag;
    Beh *bhClientService = beh_new(ag, BEH_ACTIVE,
      beh_store_done, NULL, beh_store_action, beh_store_on_end);
    agStore->clients = queue_new();
    ag->add_beh(ag, bhClientService);
  }
}

static void agent_store_msg_handler(Message *msg){
  if(msg){
    StoreMsgTags *tag = (StoreMsgTags*) msg->tag;
    if(*tag == MSG_CLIENT_ARRIVED){
      AgentStore *agStore = (AgentStore*) msg->recipient;
      Client *c = (Client*) msg->data;
      agStore->clients->push(agStore->clients, c);
      printf("Livraria:{\n"
        "  Novo cliente na fila de espera!\n"
        "  ID: %lu\n"
        "  Nome: %s %s\n"
        "  Livro Desejado: %s\n"
        "}\n\n",
        c->id,
        *(c->name), *(c->surname), *(c->bookTitle));
      }
  }
}

static void agent_store_take_down(Agent *ag){
  if(ag){
    AgentStore *agStore = (AgentStore*) ag;
    agent_destroy(ag);
    agStore->clients->free(agStore->clients, free);
    free(agStore);
  }
}
/*=======================================================================================*/

/*---------------------------------------Behaviors---------------------------------------*/
static void beh_store_action(Beh *bh){
  if(bh){
    AgentStore *agStore = (AgentStore*) bh->myAgent;
    Queue *clients = agStore->clients;
    if(!clients->empty(clients)){
      Client *c = clients->front(clients);
      if(c){
        printf("Livraria:{\n"
          "  Atendendo cliente.\n"
          "  ID: %lu\n"
          "  Nome: %s %s\n"
          "  Livro Desejado: %s\n"
          "}\n\n",
          c->id, *(c->name), *(c->surname), *(c->bookTitle));
        
        msleep(rand()%(SERVICE_TIME_RANGE+1)+SERVICE_TIME_MIN);

        printf("Livraria:{\n"
          "  Atendimento ao cliente concluído.\n"
          "  ID: %lu\n"
          "  Nome: %s %s\n"
          "  Livro Vendido: %s\n"
          "}\n\n",
          c->id, *(c->name), *(c->surname), *(c->bookTitle));
      }

      clients->pop(clients, free);
    }
  }
}

static char beh_store_done(Beh *bh){
  return bh && bh->myAgent
    ? bh->myAgent->state(bh->myAgent) == AGENT_KILLED
    : 1;
}

static void beh_store_on_end(Beh *bh){
  beh_destroy(bh);
  free(bh);
}
/*=======================================================================================*/

/*----------------------------------------Message----------------------------------------*/
Message* message_store_new(Agent *sender, Agent *recipient, StoreMsgTags sTag, void *data){
  Message *msg = (Message*) malloc(sizeof(Message));
  StoreMsgTags *tag = (StoreMsgTags*) malloc(sizeof(StoreMsgTags));
  *tag = sTag;

  *msg = ((Message){
    .sender = sender, .recipient = recipient,
    .tag = (void*) tag, .data = data,
    .destroy = message_store_destroy
  });

  return msg;
}

static void message_store_destroy(Message *msg){
  if(msg){
    free(msg->tag);
    free(msg);
  }
}
/*=======================================================================================*/