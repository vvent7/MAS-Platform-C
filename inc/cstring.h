#ifndef CSTRING_H
#define CSTRING_H

#include <stdlib.h>

#define _CSTRING_GROWTH_FACTOR 2
#define _CSTRING_DEFAULT_AUTO_SHRINK 0

#ifndef MIN
  #define MIN(x, y) (x<y ? x : y) /* Returns the minimum number (x or y). */
#endif

#ifndef MAX
  #define MAX(x, y) (x>y ? x : y) /* Returns the maximum number (x or y). */
#endif

typedef char* String;

typedef enum{CSTRING_STRING_TYPE, CSTRING_ARR_TYPE} StrType;
typedef struct{
  size_t sz, cap; /* size, capacity */
  unsigned short auto_shrink; /* automatic shrink on(1) / off(0) */
} StringData;

static const size_t string_npos = -1;

/*----------------------------------Auxiliaries----------------------------------*/
/* Verifies whether the String is valid */
unsigned short string_valid(const String *str);

/* Returns the String's Data Address */
StringData* string_data(const String *str);
/*==============================================================================*/

/*----------------------------------Constructor----------------------------------*/
/* Creates the String and returns its address */
String* string_new_cfg(unsigned short auto_shrink);
#define string_new() (string_new_cfg(_CSTRING_DEFAULT_AUTO_SHRINK))
/*==============================================================================*/

/*-----------------------------------Pointers-----------------------------------*/
/* Returns char pointer to the beginning of String */
char* string_begin(const String *str);

/* Returns char pointer to the end of String */
char* string_end(const String *str);
/*==============================================================================*/

/*----------------------------------Dimensions----------------------------------*/
/* Returns String's size */
size_t string_size(const String *str);

/* Returns String's size depending on sType.
 * sType: specifies type of 'str':
 *   - CSTRING_STRING_TYPE -> String (uses stored size / O(1))
 *   - CSTRING_ARR_TYPE -> char* (uses strlen to find size / O(n))*/
size_t string_size_by_type(const char *str, const StrType sType);

/* Returns String's Total Capacity */
size_t string_capacity(const String *str);

/* Verifies whether the String is empty */
unsigned short string_empty(const String *str);

/* Sets String's Size */
void string_set_size(String *str, const size_t new_size);

/* Sets String's Capacity */
void string_set_capacity(String *str, const size_t new_cap);

/* Sets String's auto_shrink */
void string_set_auto_shrink(String *str, const unsigned short auto_shrink);

/* Requests the container to reduce its capacity to fit its size. 
 * Shrinks in such a way that the following statement is true: 
 * (size<=capacity && size>=(capacity/_CSTRING_GROWTH_FACTOR))*/
void string_shrink_to_fit(String *str);

/*==============================================================================*/

/*--------------------------------Element Access--------------------------------*/
/* Returns the element at a specific 'index' in the vector, or '\0', if invalid */
char string_at(const String *str, const size_t index);

/* Returns the first char in the vector, or '\0' if it's empty */
char string_front(const String *str);

/* Returns the last char in the vector, or '\0' if it's empty */
char string_back(const String *str);
/*==============================================================================*/

/*-----------------------------------Modifiers-----------------------------------*/
/* Sets a gap starting at a specific 'index' with a specific 'length' in the vector.
 * Returns: char pointer to the beginning of gap in the array of the String */
char* string_set_gap(String *str, const size_t index, const size_t length);

/* Inserts 'length' elements of 's2' inside 's1' at specific 'index'
 * Returns: pointer to the beggining of inserted string */
char* string_n_insert(String *s1, const size_t index, const char *s2, const size_t length);

/* Inserts ALL elements of 's2' inside 's1' at specific 'index'
 * s2Type: specifies type of 's2':
 *   - CSTRING_STRING_TYPE -> String (uses stored size / O(1))
 *   - CSTRING_ARR_TYPE -> char* (uses strlen to find size / O(n))
 * Returns: pointer to the beggining of inserted string */
char* string_insert(String *s1, const size_t index, const char *s2, const StrType s2Type);

/* Appends 'length' elements of 's2' at the end of 's1' 
 * Returns: pointer to the beggining of appended string */
char* string_n_append(String *s1, const char *s2, const size_t length);

/* Inserts ALL elements of 's2' inside 's1' at the end of 's1'
 * s2Type: specifies type of 's2':
 *   - CSTRING_STRING_TYPE -> String (uses stored size / O(1))
 *   - CSTRING_ARR_TYPE -> char* (uses strlen to find size / O(n))
 * Returns: pointer to the beggining of appended string */
char* string_append(String *s1, const char *s2, const StrType s2Type);

/* Inserts char 'c' inside 's1' at specific 'index'
 * Returns: pointer to the inserted char */
char* string_insert_c(String *str, const size_t index, const char c);

/* Appends char 'c' at the end of 's1' 
 * Returns: pointer to the inserted char */
char* string_append_c(String *str, const char c);

/* Removes 'length' elements from 'str' starting at a specific index
 * Returns: 
 * - Successful: first element after the last erased
 * - Error: NULL */
char* string_n_erase(String *str, const size_t index, size_t length);

/* Removes a single element from 'str' at specific index
 * Returns: 
 * - Successful: first element after the last erased
 * - Error: NULL */
char* string_erase(String *str, const size_t index);

/* Clears String's content */
void string_clear(String *str);

/* Frees all memory associated to the string and its pointer */
void string_free(String *str);
/*===============================================================================*/

#endif