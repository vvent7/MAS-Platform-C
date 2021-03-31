#include <stdlib.h>
#include "cstring.h"
#include "client.h"

Client* client_new(size_t id, String *name, String *surname, String *bookTitle){
  Client *c = (Client*) malloc(sizeof(Client));
  c->id = id; c->name = name; c->surname = surname; c->bookTitle = bookTitle;
  return c;
}