#ifndef DATABASE_H
#define DATABASE_H

#include "cvector.h"
#include "cstring.h"

typedef Vector(String*) VecStr;

typedef struct {
  String *dbFile;
  VecStr *names, *surnames, *bookTitles;
} Database;

/* Loads the database from scratch */
Database* database_load(char *dbFile);

/* Returns the address of database, or NULL if not successful.
 * Database loaded previously: returns the last loaded database
 * Database never loaded: calls database_load and returns its result */
Database* database_get(char *dbFile);

/* Frees all memory occupied by the database */
void database_free();

#endif