#ifndef CSTRING_H
#define CSTRING_H

#include <stdlib.h>

#ifndef _CSTR_MIN
  #define _CSTR_MIN(x, y) (x<y ? x : y) /* Returns the minimum number (either x or y). */
#endif

#ifndef _CSTR_MAX
  #define _CSTR_MAX(x, y) (x>y ? x : y) /* Returns the maximum number (either x or y). */
#endif

/* Rules:
 *   _CSTR_GROWTH_FACTOR > 1
 *   _CSTR_DEF_CAP_MAN must be a value of StringCapMan enum
 *   0 <= _CSTR_DEF_AUTO_SK <= 1 */
#define _CSTR_GROWTH_FACTOR 2
#define _CSTR_DEF_CAP_MAN CSTR_CAPMAN_LOG
#define _CSTR_DEF_AUTO_SK 0

/* StringCapMan manages the way how the String Grows and Shrinks.
 * ## CSTR_CAPMAN_LOG ##
 *  - Growing rule: (size<=capacity && (capacity % _CSTR_GROWTH_FACTOR)==0)
 *  - Shrinking rule: (size<=capacity && size>=(capacity/_CSTR_GROWTH_FACTOR))
 * ## CSTR_CAPMAN_EQ_SZ ##
 *  - Growing rule: (size==capacity)
 *  - Shrinking rule: (size==capacity) */
typedef enum{CSTR_CAPMAN_LOG, CSTR_CAPMAN_EQ_SZ} StringCapMan;

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
  StringCapMan capMan;
  char auto_shrink;
} StringData;
typedef char* String;

static const size_t string_npos = -1;

/*----------------------------------Auxiliaries----------------------------------*/
/* Verifies whether the String is valid */
char string_valid(const String *str);
/*===============================================================================*/

/*----------------------------------Constructor----------------------------------*/
/* Creates an empty String and returns its address */
String* string_new_cfg(StringCapMan capMan, char auto_shrink);
#define string_new() (string_new_cfg(_CSTR_DEF_CAP_MAN, _CSTR_DEF_AUTO_SK))

/* Creates a String with the same content of 'src' and returns its address */
String* string_new_copy_cfg(const char *src, const StrType srcType, StringCapMan capMan, char auto_shrink);
#define string_new_copy(src, srcType) (string_new_copy_cfg(src, srcType, _CSTR_DEF_CAP_MAN, _CSTR_DEF_AUTO_SK))
/*==============================================================================*/

/*----------------------------------Misc Info----------------------------------*/
/* Returns pointer to the StringData
 * OBS:
 *  - This should not be used by the user, just by the API's functions.
 *    Incorrect use of this function can cause undefined behaviour.
 *  - The returned Pointer can become invalid if the actual address of
 *    StringData is modified by other functions (e.g., erase and insert) */
StringData* _string_data(const String *str);
#define _string_data_raw(str) (((StringData*)(*str))-1)

/* Returns char pointer to the beginning of String */
char* string_begin(const String *str);

/* Returns char pointer to the end of String */
char* string_end(const String *str);

/* Returns String's Capacity Management */
StringCapMan string_cap_man(String *str);

/* Sets String's Capacity Management */
void string_set_cap_man(String *str, StringCapMan capMan);

/* Returns String's auto_shrink */
char string_auto_shrink(String *str);

/* Sets String's auto_shrink */
void string_set_auto_shrink(String *str, char auto_shrink);
/*==============================================================================*/

/*----------------------------------Dimensions----------------------------------*/
/* Verifies whether the String is empty */
char string_empty(const String *str);

/* Returns String's size */
size_t string_size(const String *str);

/* Returns String's size depending on sType */
size_t string_size_by_type(const char *str, const StrType sType);

/* Sets String's Size
 * OBS: This shouldn't be used by the user, just by the API's functions.
 *      Instead, use insert and erase functions to increase and decrease
 *      String's size.
 *      Incorrect use of this function can cause undefined behaviour. */
void _string_set_size(String *str, const size_t new_size);

/* Returns String's Total Capacity */
size_t string_capacity(const String *str);

/* Sets String's Capacity
   OBS: if new_cap < size, the String's size will be decreased */
void string_set_capacity(String *str, const size_t new_cap);

/* Requests the container to reduce its capacity to fit its size, based on
 * StringCapMan of the String 'str'*/
void string_shrink(String *str);
/*==============================================================================*/

/*--------------------------------Element Access--------------------------------*/
/* Returns the element at a specific 'index' in the String, or '\0', if invalid */
char string_at(const String *str, const size_t index);

/* Returns the first char in the String, or '\0' if it's empty */
char string_front(const String *str);

/* Returns the last char in the String, or '\0' if it's empty */
char string_back(const String *str);
/*==============================================================================*/

/*-----------------------------------Modifiers-----------------------------------*/
/* Sets a gap starting at a specific 'index' with a specific 'length' in the String.
 * Returns: char pointer to the beginning of gap in the array of the String */
char* string_set_gap(String *str, const size_t index, const size_t length);

/* OBS to all insert/append functions:
 *   1) No checks are performed to check the value of 'length' and 'index';
 *   2) No checks are performed to check if 's2' is null-terminated.
 *      Sending invalid values will result in undefined behavior
 *   3) Return: success -> Pointer to first element inserted in the String
 *              failure -> NULL */

/* Inserts 'length' elements of 's2' inside 's1' at specific 'index' */
char* string_insert_n(String *s1, const size_t index, const char *s2, const size_t length);

/* Inserts ALL elements of 's2' inside 's1' at specific 'index'
 * s2Type: specifies type of 's2' */
#define string_insert_all(s1, index, s2, s2Type) (string_insert_n(s1, index, s2, string_size_by_type(s2, s2Type)))

/* Inserts char 'c' inside 's1' at specific 'index' */
char* string_insert_one(String *str, const size_t index, const char c);

/* Appends 'length' elements of 's2' at the end of 's1' */
#define string_append_n(s1, s2, length) (string_insert_n(s1, string_size(s1), s2, length))

/* Inserts ALL elements of 's2' at the end of 's1'
 * s2Type: specifies type of 's2' */
#define string_append_all(s1, s2, s2Type) (string_insert_n(s1, string_size(s1), s2, string_size_by_type(s2, s2Type)))

/* Appends char 'c' at the end of 's1' */
#define string_append_one(str, c) (string_insert_one(str, string_size(str), c))

/* Copies ALL elements of 's2' to 's1', replacing its content
 * s2Type: specifies type of 's2'
 * Returns: pointer to the beggining of string 's1' */
char* string_copy(String *s1, const char *s2, const StrType s2Type);

void string_trim(char *str, StrType sType);

/* OBS to all erase functions:
 *   1) all erase requests with 'index' out of bounds will fail
 *   2) all erase requests with 'index' in bounds, but 'length' out of
 *      bounds will be executed just with the inbounds 'indexes'.
 *   3) Return: Success -> A pointer to the new location of the element that followed
 *              the last  element erased by the function call. This is the String end if
 *              the operation erased the last element in the sequence.
 *              Failure -> NULL */

/* Removes 'length' elements from 'str' starting at a specific index */
char* string_erase_n(String *str, const size_t index, size_t length);

/* Removes a single element from 'str' at specific index */
#define string_erase(str, index) (string_erase_n(str, index, 1))

/* Clears String's content */
void string_clear(String *str);

/* Frees all memory associated to the String and its pointer */
void string_free(String *str);
/*===============================================================================*/

#endif