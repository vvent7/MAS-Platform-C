#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cstring.h"
#include "database.h"
#define CHAR_BUFFER_SZ 101

static Database *db = NULL;

static void database_next_line(FILE *stream){
  char c;
  while((c=fgetc(stream))!=EOF && c!='\n'){}
}

static char* database_fgets(char *str, size_t n, FILE *stream){
  if(fgets(str, n, stream)){
    if(str[strlen(str)-1]!='\n') database_next_line(stream);
    string_trim(str, CSTRING_TP_ARR);
    return str;
  }
  return NULL;
}

Database* database_load(char *dbFile){
  FILE *file = fopen(dbFile, "r");

  if(file){
    char buffer[CHAR_BUFFER_SZ];
    String *aux;

    database_free();
    
    db = malloc(sizeof(Database));
    db->dbFile = string_new_copy_cfg(dbFile, CSTRING_TP_ARR, CSTR_CAPMAN_EQ_SZ, 1);
    db->names = vector_new_cfg(String*, CVEC_CAPMAN_LOG, 1);
    db->surnames = vector_new_cfg(String*, CVEC_CAPMAN_LOG, 1);
    db->bookTitles = vector_new_cfg(String*, CVEC_CAPMAN_LOG, 1);

    while(database_fgets(buffer, CHAR_BUFFER_SZ, file) && buffer[0]!='\0'){
      aux = string_new_copy_cfg(buffer, CSTRING_TP_ARR, CSTR_CAPMAN_EQ_SZ, 1);
      vector_append_one(db->names, &aux);
    }

    while(database_fgets(buffer, CHAR_BUFFER_SZ, file) && buffer[0]!='\0'){
      aux = string_new_copy_cfg(buffer, CSTRING_TP_ARR, CSTR_CAPMAN_EQ_SZ, 1);
      vector_append_one(db->surnames, &aux);
    }

    while(database_fgets(buffer, CHAR_BUFFER_SZ, file) && buffer[0]!='\0'){
      aux = string_new_copy_cfg(buffer, CSTRING_TP_ARR, CSTR_CAPMAN_EQ_SZ, 1);
      vector_append_one(db->bookTitles, &aux);
    }

    fclose(file);

    if(vector_empty(db->names) || vector_empty(db->surnames) || vector_empty(db->bookTitles)){
      database_free();
      return NULL;
    }
  
    vector_set_cap_man(db->names, CSTR_CAPMAN_EQ_SZ);
    vector_set_cap_man(db->surnames, CSTR_CAPMAN_EQ_SZ);
    vector_set_cap_man(db->bookTitles, CSTR_CAPMAN_EQ_SZ);
    
    return db;
  }
  return NULL;
}

Database* database_get(char *dbFile){
  return db && strcmp(*(db->dbFile), dbFile)==0
    ? db
    : database_load(dbFile);
}

static void database_vector_string_freeData(Vector(String*) *vec){
  size_t sz = vector_size(vec), i;
  for(i=0;i<sz;i++) string_free((*vec)[i]);
  vector_free(vec);
}

void database_free(){
  if(db){
    string_free(db->dbFile);
    database_vector_string_freeData(db->names);
    database_vector_string_freeData(db->surnames);
    database_vector_string_freeData(db->bookTitles);
    free(db);
    db = NULL;
  }
}
