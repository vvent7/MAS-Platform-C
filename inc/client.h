#ifndef CLIENT_H
#define CLIENT_H

#include "cstring.h"

typedef struct{
  size_t id;
  String *name, *surname, *bookTitle;
}Client;

Client* client_new(size_t id, String *name, String *surname, String *bookTitle);

#endif