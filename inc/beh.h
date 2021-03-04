#ifndef BEH_H
#define BEH_H

typedef struct Agent Agent;

/* Beh -> Behaviour */
typedef struct Beh{
/* public attributes */
  Agent *myAgent;

/* private methods */
  void (*blocked)();
  void (*unblocked)();

/* public methods */
  void (*action)(struct Beh *bh);
  unsigned short (*done)(struct Beh *bh);
  void (*on_start)(struct Beh *bh);
  void (*on_end)(struct Beh *bh);
} Beh;

Beh* beh_new(Agent *myAgent, void (*action)(struct Beh *bh),
  unsigned short (*done)(struct Beh *bh),
  void (*on_start)(struct Beh *bh),
  void (*on_end)(struct Beh *bh));

void blocked(Beh *bh);

void unblocked(Beh *bh);

void beh_action(Beh *bh);

unsigned short beh_done(Beh *bh);

void beh_on_start(Beh *bh);

void beh_on_end(Beh *bh);

#endif