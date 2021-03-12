#ifndef CSTRING_H
#define CSTRING_H

#include <stdlib.h>

#ifndef MIN
  #define MIN(x, y) (x<y ? x : y) /* Returns the minimum number (x or y). */
#endif

#ifndef MAX
  #define MAX(x, y) (x>y ? x : y) /* Returns the maximum number (x or y). */
#endif

#define _CSTRING_GROWTH_FACTOR 2
#define _CSTRING_DEFAULT_CAP_MAN 0
#define _CSTRING_DEFAULT_AUTO_SHRINK 0

/* CapacityManagement manages the way how the String Grows and Shrinks.
 * ## CSTRING_CAPMAN_LOGARITHMIC ##
 *  - Growing rule: (size<=capacity && (capacity % _CSTRING_GROWTH_FACTOR)==0)
 *  - Shrinking rule: (size<=capacity && size>=(capacity/_CSTRING_GROWTH_FACTOR))
 * ## CSTRING_CAPMAN_EQUAL_SIZE ##
 *  - Growing rule: (size==capacity)
 *  - Shrinking rule: (size==capacity) */
typedef enum{CSTRING_CAPMAN_LOGARITHMIC, CSTRING_CAPMAN_EQUAL_SIZE} CapacityManagement;

/* StrType is useful for identifying what type of string is being passed
 * for a function (either array of chars or String). StrType is useful
 * when performing some actions, like finding the length of a string:
 *   - CSTRING_TP_STRING -> String (uses stored size / O(1))
 *   - CSTRING_TP_ARR -> char* (uses strlen to find size / O(n)) */
typedef enum{CSTRING_TP_STRING, CSTRING_TP_ARR} StrType;

/* StringData holds the data of a String, such as:
 * - Size and Capacity;
 * - Auto Shrink (on/off);
 * - Capacity Management. */
typedef struct{
  size_t sz, cap;
  char auto_shrink;
  CapacityManagement capMan;
} StringData;
typedef char* String;

static const size_t string_npos = -1;

/*----------------------------------Auxiliaries----------------------------------*/
/* Verifies whether the String is valid */
char string_valid(const String *str);

/* Returns pointer to the StringData
 * OBS:
 *  - This should not be used by the user, just by the API's functions.
 *    Incorrect use of this function can cause undefined behaviour.
 *  - The returned Pointer can become invalid if the actual address of
 *    StringData is modified by other functions (e.g., erase and insert) */
StringData* _string_data(const String *str);
/*==============================================================================*/

/*----------------------------------Constructor----------------------------------*/
/* Creates an empty String and returns its address */
String* string_new_cfg(CapacityManagement capMan, char auto_shrink);
#define string_new() (string_new_cfg(_CSTRING_DEFAULT_CAP_MAN, _CSTRING_DEFAULT_AUTO_SHRINK))

/* Creates a String with the same content of 'src' and returns its address */
String* string_new_copy_cfg(const char *src, const StrType srcType, CapacityManagement capMan, char auto_shrink);
#define string_new_copy(src, srcType) (string_new_copy_cfg(src, srcType, _CSTRING_DEFAULT_CAP_MAN, _CSTRING_DEFAULT_AUTO_SHRINK))
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

/* Returns String's size depending on sType */
size_t string_size_by_type(const char *str, const StrType sType);

/* Returns String's Total Capacity */
size_t string_capacity(const String *str);

/* Verifies whether the String is empty */
char string_empty(const String *str);

/* Sets String's Size
 * OBS: This shouldn't be used by the user, just by the API's functions.
 *      Instead, use insert and erase functions to increase and decrease
 *      String's size.
 *      Incorrect use of this function can cause undefined behaviour. */
void _string_set_size(String *str, const size_t new_size);

/* Sets String's Capacity
   OBS: if new_cap < size, the String's size will be decreased */
void string_set_capacity(String *str, const size_t new_cap);

/* Sets String's auto_shrink */
void string_set_auto_shrink(String *str, const char auto_shrink);

/* Requests the container to reduce its capacity to fit its size, based on
 * CapacityManagement of the String 'str'*/
void string_shrink(String *str);

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
char* string_n_insert(String *s1, const size_t index, const char *s2, size_t length, const StrType s2Type);

/* Inserts ALL elements of 's2' inside 's1' at specific 'index'
 * s2Type: specifies type of 's2'
 * Returns: pointer to the beggining of inserted string */
#define string_insert(s1, index, s2, s2Type) (string_n_insert(s1, index, s2, string_npos, s2Type))

/* Appends 'length' elements of 's2' at the end of 's1' 
 * Returns: pointer to the beggining of appended string */
#define string_n_append(s1, s2, length, s2Type) (string_n_insert(s1, string_size(s1), s2, length, s2Type))

/* Inserts ALL elements of 's2' inside 's1' at the end of 's1'
 * s2Type: specifies type of 's2'
 * Returns: pointer to the beggining of appended string */
#define string_append(s1, s2, s2Type) (string_n_insert(s1, string_size(s1), s2, string_npos, s2Type))

/* Inserts char 'c' inside 's1' at specific 'index'
 * Returns: pointer to the inserted char */
char* string_insert_c(String *str, const size_t index, const char c);

/* Appends char 'c' at the end of 's1' 
 * Returns: pointer to the inserted char */
#define string_append_c(str, c) (string_insert_c(str, string_size(str), c))

/* Copies ALL elements of 's2' to 's1', replacing its content
 * s2Type: specifies type of 's2'
 * Returns: pointer to the beggining of string 's1' */
char* string_copy(String *s1, const char *s2, const StrType s2Type);

/* Removes 'length' elements from 'str' starting at a specific index
 * Returns:
 *  - Successful: first element after the last erased
 *  - Error: NULL */
char* string_n_erase(String *str, const size_t index, size_t length);

/* Removes a single element from 'str' at specific index
 * Returns: 
 * - Successful: first element after the last erased
 * - Error: NULL */
#define string_erase(str, index) (string_n_erase(str, index, 1))

/* Clears String's content */
void string_clear(String *str);

/* Frees all memory associated to the string and its pointer */
void string_free(String *str);
/*===============================================================================*/

#endif