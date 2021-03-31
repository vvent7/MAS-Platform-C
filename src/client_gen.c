#include <stdio.h>
#include <stdlib.h>
#include "cvector.h"
#include "cstring.h"
#include "database.h"
#include "mas.h"
#include "client.h"
#include "client_gen.h"
#include "store.h"
#include "misc.h"

#define DATABASE_FILE "database.txt"

#define GEN_TIME_MIN 500
#define GEN_TIME_RANGE 3000

/*--------------------------------------Prototypes--------------------------------------*/
static void agent_client_gen_setup(Agent *ag);
static void agent_client_gen_take_down(Agent *ag);

static void beh_client_gen_action(Beh *bh);
static char beh_client_gen_done(Beh *bh);
static void beh_client_gen_on_end(Beh *bh);
/*======================================================================================*/

/*-----------------------------------------Agent-----------------------------------------*/
AgentClientGen* agent_client_gen_new(const char *aid, const char *name, AgentStore *agStore){
  if(agStore){
    AgentClientGen *acg = (AgentClientGen*) malloc(sizeof(AgentClientGen));
    agent_init(&(acg->super), aid, name, agent_client_gen_setup,
      NULL, agent_client_gen_take_down);
    acg->agStore = agStore;
    return acg;
  }
  return NULL;
}

static void agent_client_gen_setup(Agent *ag){
  Beh *bhCreateClient = beh_new(ag, BEH_ACTIVE, beh_client_gen_done, NULL,
    beh_client_gen_action, beh_client_gen_on_end);
  ag->add_beh(ag, bhCreateClient);
}

static void agent_client_gen_take_down(Agent *ag){
  agent_destroy(ag);
  free((AgentClientGen*) ag);
}
/*=======================================================================================*/

/*---------------------------------------Behaviors---------------------------------------*/
static void beh_client_gen_action(Beh *bh){
  static size_t genCount = 0;
  AgentStore *agStore = ((AgentClientGen*)bh->myAgent)->agStore;

  if(agStore->super.state((Agent*)agStore)==AGENT_RUNNING){
    Database *db = database_get(DATABASE_FILE);
    size_t indName = rand()%vector_size(db->names),
      indSurname = rand()%vector_size(db->surnames),
      indBookTitle = rand()%vector_size(db->bookTitles);
    Client *c;

    if(genCount) msleep(rand()%(GEN_TIME_RANGE+1)+GEN_TIME_MIN);
    c = client_new(genCount++, (*(db->names))[indName],
      (*(db->surnames))[indSurname], (*(db->bookTitles))[indBookTitle]);
    
    printf("Gerador de Clientes{\n"
      "  Novo cliente gerado!\n"
      "  ID: %lu\n"
      "  Nome: %s %s\n"
      "  Livro Desejado: %s\n"
      "}\n\n",
      c->id, *(c->name), *(c->surname), *(c->bookTitle));

    bh->myAgent->send(message_store_new(bh->myAgent, (Agent*) agStore, MSG_CLIENT_ARRIVED, c));
  }
}

static char beh_client_gen_done(Beh *bh){
  return bh && bh->myAgent
    ? bh->myAgent->state(bh->myAgent) == AGENT_KILLED
    : 1;
}

static void beh_client_gen_on_end(Beh *bh){
  beh_destroy(bh);
  free(bh);
}
/*=======================================================================================*/