#ifndef BEH_H
#define BEH_H

typedef struct Agent Agent;

/* Beh -> Behaviour */
typedef struct Beh{
/* Visible */
  Agent *myAgent;

  /* const methods */
  void (* const blocked)(struct Beh *this);
  void (* const unblocked)(struct Beh *this);

  /* changeable methods */
  void (*action)(struct Beh *this);
  char (*done)(struct Beh *this);
  void (*on_start)(struct Beh *this);
  void (*on_end)(struct Beh *this);
} Beh;

Beh beh_new(Agent *myAgent, void (*action)(struct Beh *bh),
  char (*done)(struct Beh *bh),
  void (*on_start)(struct Beh *bh),
  void (*on_end)(struct Beh *bh));

Beh* beh_new_ptr(Agent *myAgent, void (*action)(struct Beh *bh),
  char (*done)(struct Beh *bh),
  void (*on_start)(struct Beh *bh),
  void (*on_end)(struct Beh *bh));

void beh_blocked(Beh *bh);

void beh_unblocked(Beh *bh);

#endif