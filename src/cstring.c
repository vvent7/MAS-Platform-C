#include<stdlib.h>
#include<string.h>
#include "cstring.h"

unsigned short string_valid(const String *str){
  return (str!=NULL && (*str)!=NULL) ? 1 : 0;
}

StringData* string_data(const String *str){
  return string_valid(str) ? ((StringData*)(*str))-1 : NULL;
}

String* string_new_cfg(unsigned short auto_shrink){
  StringData *sd = (StringData*) malloc(sizeof(StringData) + sizeof(char));
  String *pstr = (String*) malloc(sizeof(String*));

  sd->sz = sd->cap = 0; sd->auto_shrink = auto_shrink ? 1 : 0;
  *pstr = (String) (sd+1); (*pstr)[0] = '\0';

  return pstr;
}

char* string_begin(const String *str){
  return string_valid(str) ? (*str) : NULL;
}

char* string_end(const String *str){
  return string_valid(str) ? (*str) + (string_data(str))->sz : NULL;
}

size_t string_size(const String *str){
  StringData *sd = string_data(str);
  return sd ? sd->sz : (size_t)0;
}

size_t string_size_by_type(const char *str, const StrType type){
  if(str==NULL) return 0;
  switch(type){
    case CSTRING_STRING_TYPE: return string_size((String*)(&str));
    case CSTRING_ARR_TYPE: return strlen(str);
    default: return 0;
  }
}

size_t string_capacity(const String *str){
  StringData *sd = string_data(str);
  return sd ? sd->cap : (size_t)0;
}

unsigned short string_empty(const String *str){
  return (string_size(str)==0);
}

void string_set_size(String *str, const size_t new_size){
  StringData *sd = string_data(str);

  if(sd==NULL || new_size>=string_npos) return;
  
  if(new_size!=sd->sz){
    size_t old_size = sd->sz;
    if(new_size > sd->cap){
      size_t new_cap = MAX(sd->cap, 1);
      while(new_size > new_cap) new_cap *= _CSTRING_GROWTH_FACTOR;
      string_set_capacity(str, new_cap);
      sd = ((StringData*)(*str))-1;
    }
    (*str)[old_size] = ' '; (*str)[sd->sz = new_size] = '\0';
    if(new_size < old_size && sd->auto_shrink)
      string_shrink_to_fit(str);
  }
}

void string_set_capacity(String *str, const size_t new_cap){
  StringData *sd = string_data(str);

  if(sd==NULL || new_cap>=string_npos) return;

  if(new_cap!=sd->cap){
    sd = realloc(sd, sizeof(StringData) + sizeof(char) * (new_cap + 1));
    (*str) = (String) (sd+1);

    if(sd->cap < new_cap) (*str)[sd->cap] = ' ';
    (*str)[sd->cap = new_cap] = '\0';
    sd->sz = MIN(sd->cap, sd->sz);
  }
}

void string_set_auto_shrink(String *str, unsigned short auto_shrink){
  StringData *sd = string_data(str);
  if(sd && (sd->auto_shrink = (auto_shrink ? 1 : 0)))
    string_shrink_to_fit(str);
}

void string_shrink_to_fit(String *str){
  StringData *sd = string_data(str);
  size_t new_cap;

  if(sd==NULL) return;
  
  new_cap = sd->cap;
  while(sd->sz < new_cap) new_cap /= _CSTRING_GROWTH_FACTOR;
  string_set_capacity(str, new_cap*_CSTRING_GROWTH_FACTOR);
}

char string_at(const String *str, const size_t index){
  return index < string_size(str) ? (*str)[index] : '\0';
}

char string_front(const String *str){
  return string_empty(str) ? '\0' : (*str)[0];
}

char string_back(const String *str){
  size_t sz;
  return (sz = string_size(str))==0 ? '\0' : (*str)[sz-1];
}

char* string_set_gap(String *str, const size_t index, const size_t length){
  StringData *sd = string_data(str);
  size_t to_move;
  char *begin, *end;

  if(sd==NULL || index>sd->sz) return NULL;

  to_move = (sd->sz - index) * sizeof(char);
  string_set_size(str, sd->sz+length);
  begin = (*str) + index; end = begin + length;
  
  memmove(end, begin, to_move);

  return begin;
}

char* string_n_insert(String *s1, const size_t index, const char *s2, const size_t length){
  StringData *sd = string_data(s1);
  char *temp, *dest;

  if(sd==NULL || index>sd->sz || s2==NULL) return NULL;

  temp = malloc(sizeof(char)*(length+1)); strncpy(temp, s2, length);
  dest = string_set_gap(s1, index, length); strncpy(dest, temp, length); 
  
  free(temp);

  return dest;
}

char* string_insert(String *s1, const size_t index, const char *s2, const StrType s2Type){
  return string_n_insert(s1, index, s2, string_size_by_type(s2, s2Type));
}

char* string_n_append(String *s1, const char *s2, const size_t length){
  return string_n_insert(s1, string_size(s1), s2, length);
}

char* string_append(String *s1, const char *s2, const StrType s2Type){
  return string_n_insert(s1, string_size(s1), s2, string_size_by_type(s2, s2Type));
}

char* string_insert_c(String *str, const size_t index, const char c){
  char *dest;
  if((dest = string_set_gap(str, index, 1))==NULL) return NULL;
  (*dest) = c;
  return dest;
}

char* string_append_c(String *str, const char c){
  return string_insert_c(str, string_size(str), c);
}

char* string_n_erase(String *str, const size_t index, size_t length){
  StringData *sd = string_data(str);
  char *begin, *end;

  if(sd==NULL || index>=sd->sz) return NULL;

  length = MIN(length, sd->sz-index);
  begin = (*str) + index; end = begin + length;

  memmove(begin, end, (sd->sz-index-length) * sizeof(char));
  string_set_size(str, sd->sz-length);

  return (*str)+index;
}

char* string_erase(String *str, const size_t index){
  return string_n_erase(str, index, 1);
}

void string_clear(String *str){
  string_set_size(str, 0);
}

void string_free(String *str){
  free(string_data(str));
  free(str);
}