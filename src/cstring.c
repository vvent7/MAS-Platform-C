#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cstring.h"

/*----------------------------------Auxiliaries----------------------------------*/
char string_valid(const String *str){
  return (str!=NULL && (*str)!=NULL) ? 1 : 0;
}
/*===============================================================================*/

/*----------------------------------Constructor----------------------------------*/
String* string_new_cfg(StringCapMan capMan, char auto_shrink){
  StringData *sd = (StringData*) malloc(sizeof(StringData) + sizeof(char));
  String *pstr = (String*) malloc(sizeof(String));

  sd->sz = sd->cap = 0;
  sd->capMan = CSTR_CAPMAN_LOG<=capMan && capMan<=CSTR_CAPMAN_EQ_SZ ? capMan : _CSTR_DEF_CAP_MAN;
  sd->auto_shrink = auto_shrink ? 1 : 0;
  *pstr = (String) (sd+1); (*pstr)[0] = '\0';

  return pstr;
}

String* string_new_copy_cfg(const char *src, const StrType srcType, StringCapMan capMan, char auto_shrink){
  String *str = string_new_cfg(capMan, auto_shrink);
  string_append_all(str, src, srcType);
  return str;
}
/*==============================================================================*/

/*----------------------------------Misc Info----------------------------------*/
StringData* _string_data(const String *str){
  return string_valid(str) ? _string_data_raw(str) : NULL;
}

char* string_begin(const String *str){
  return string_valid(str) ? (*str) : NULL;
}

char* string_end(const String *str){
  StringData *sd = _string_data(str);
  return sd ? (*str) + sd->sz : NULL;
}

StringCapMan string_cap_man(String *str){
  StringData *sd = _string_data(str);
  return sd ? sd->capMan : _CSTR_DEF_CAP_MAN;
}

void string_set_cap_man(String *str, StringCapMan capMan){
  StringData *sd = _string_data(str);

  if(sd){
    sd->capMan = CSTR_CAPMAN_LOG<=capMan && capMan<=CSTR_CAPMAN_EQ_SZ ? capMan : _CSTR_DEF_CAP_MAN;
    if(sd->auto_shrink) string_shrink(str);
  }
}

char string_auto_shrink(String *str){
  StringData *sd = _string_data(str);
  return sd ? sd->auto_shrink : _CSTR_DEF_AUTO_SK;
}

void string_set_auto_shrink(String *str, char auto_shrink){
  StringData *sd = _string_data(str);
  if(sd && (sd->auto_shrink = (auto_shrink ? 1 : 0)))
    string_shrink(str);
}
/*==============================================================================*/

/*----------------------------------Dimensions----------------------------------*/
char string_empty(const String *str){
  return (string_size(str)==0);
}

size_t string_size(const String *str){
  StringData *sd = _string_data(str);
  return sd ? sd->sz : (size_t)0;
}

size_t string_size_by_type(const char *str, const StrType type){
  if(str==NULL) return 0;
  switch(type){
    case CSTRING_TP_STRING: return string_size((String*)(&str));
    case CSTRING_TP_ARR: return strlen(str);
    default: return 0;
  }
}

void _string_set_size(String *str, const size_t new_size){
  StringData *sd = _string_data(str);

  if(sd && new_size!=sd->sz){
    if(new_size > sd->cap){
      size_t new_cap;
      
      if(sd->capMan==CSTR_CAPMAN_LOG){
        new_cap = _CSTR_MAX(sd->cap, 1);
        while(new_size > new_cap) new_cap *= _CSTR_GROWTH_FACTOR;
      }
      else new_cap = new_size;

      string_set_capacity(str, new_cap);
      _string_data_raw(str)->sz = new_size;
    }
    else{
      sd->sz = new_size;
      if(sd->auto_shrink)
        string_shrink(str);
    }
    (*str)[new_size]='\0';
  }
}

size_t string_capacity(const String *str){
  StringData *sd = _string_data(str);
  return sd ? sd->cap : (size_t)0;
}

void string_set_capacity(String *str, const size_t new_cap){
  StringData *sd = _string_data(str);

  if(sd && new_cap<string_npos && new_cap!=sd->cap){
    sd = realloc(sd, sizeof(StringData) + sizeof(char) * (new_cap + 1));
    (*str) = (String) (sd+1);

    if(sd->cap < new_cap) (*str)[sd->cap] = ' ';
    (*str)[sd->cap = new_cap] = '\0';
    sd->sz = _CSTR_MIN(sd->cap, sd->sz);
  }
}

void  string_shrink(String *str){
  StringData *sd = _string_data(str);
  if(sd){
    size_t new_cap;

    if(sd->capMan==CSTR_CAPMAN_LOG){
      if(sd->sz==0) new_cap = 0;
      else{
        size_t next_cap = new_cap = sd->cap;
        while(sd->sz<=(next_cap/=_CSTR_GROWTH_FACTOR)) new_cap = next_cap;
      }
    }
    else new_cap = sd->sz;

    string_set_capacity(str, new_cap);
  }
}
/*==============================================================================*/

/*--------------------------------Element Access--------------------------------*/
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
/*==============================================================================*/

/*-----------------------------------Modifiers-----------------------------------*/
char* string_set_gap(String *str, const size_t index, const size_t length){
  StringData *sd = _string_data(str);

  if(sd && index<=sd->sz){
    size_t to_move = (sd->sz - index) * sizeof(char);
    char *begin, *end;

    _string_set_size(str, sd->sz+length);

    begin = (*str) + index;
    end = begin + length;
    
    memmove(end, begin, to_move);

    return begin;
  }
  return NULL;
}

char* string_insert_n(String *s1, const size_t index, const char *s2, const size_t length){
  StringData *sd = _string_data(s1);

  if(sd && index<=sd->sz && s2){
    char *temp = malloc(sizeof(char)*(length)), *dest;

    memcpy(temp, s2, length);
    dest = string_set_gap(s1, index, length);
    memcpy(dest, temp, length);
    
    free(temp);

    return dest;
  }

  return NULL;  
}

char* string_insert_one(String *str, const size_t index, const char c){
  char *dest;
  if((dest = string_set_gap(str, index, 1))==NULL) return NULL;
  (*dest) = c;
  return dest;
}

char* string_copy(String *s1, const char *s2, const StrType s2Type){
  if(string_valid(s1)){
    size_t length = string_size_by_type(s2, s2Type);
    if(length>0){
      _string_set_size(s1, length);
      memmove(*s1, s2, length);
    }
    return (*s1);
  }
  return NULL;
}

void string_trim(char *str, StrType sType){
  size_t first=string_npos, last=first, i;

  for(i=0;str[i]!='\0' && first==string_npos;i++)
    if(isprint(str[i]) && !isspace(str[i]))
      first = last = i;
  
  for(;str[i]!='\0';i++)
    if(isprint(str[i]) && !isspace(str[i]))
      last = i;

  if(sType==CSTRING_TP_STRING){
    size_t new_size = 0;
    if(first!=string_npos && last!=string_npos){
      new_size = last-first+1;
      memmove(str, str+first, new_size * sizeof(char));
      str[new_size]='\0';
    }
    _string_set_size((String*)(&str), new_size);
  }
  else{
    if(first==string_npos || last==string_npos) str[0]='\0';
    else{
      size_t new_size = (last-first+1);
      memmove(str, str+first, new_size * sizeof(char));
      str[new_size]='\0';
    }
  }
}

char* string_erase_n(String *str, const size_t index, size_t length){
  StringData *sd = _string_data(str);

  if(sd && index<sd->sz){
    char *begin = (*str) + index,
      *end = begin + (length = _CSTR_MIN(length, sd->sz-index));

    memmove(begin, end, (sd->sz-index-length) * sizeof(char));
    _string_set_size(str, sd->sz-length);

    return (*str)+index;
  }
  return NULL;
}

void string_clear(String *str){
  _string_set_size(str, 0);
}

void string_free(String *str){
  free(_string_data(str)); free(str);
}
/*===============================================================================*/