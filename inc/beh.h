#ifndef BEH_H
#define BEH_H

typedef struct Agent Agent;

/* Beh -> Behaviour */
typedef struct Beh{
/* public attributes */
  Agent *myAgent;
  unsigned short dn;

/* public methods */
  void (*free)(struct Beh *b);
  void (*action)(struct Beh *b);
  unsigned short (*done)(struct Beh *b);
  void (*on_start)(struct Beh *b);
  void (*on_end)(struct Beh *b);
} Beh;

Beh* beh_new(Agent *myAgent);

void beh_freeData(void *x);

void beh_free(Beh *bh);

void blocked(Beh *bh);

void unblocked(Beh *bh);

void beh_action(Beh *bh);

unsigned short beh_done(Beh *bh);

void beh_on_start(Beh *bh);

void beh_on_end(Beh *bh);

#endif