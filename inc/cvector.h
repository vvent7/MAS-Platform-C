#ifndef CVECTOR_H
#define CVECTOR_H

#include <stdlib.h>

#ifndef _CVEC_MIN
  #define _CVEC_MIN(x, y) (x<y ? x : y) /* Returns the minimum number (x or y). */
#endif

#ifndef _CVEC_MAX
  #define _CVEC_MAX(x, y) (x>y ? x : y) /* Returns the maximum number (x or y). */
#endif

/* Rules:
 *   _CVEC_GROWTH_FACTOR > 1
 *   _CVEC_DEF_CAP_MAN must be a value of VectorCapMan enum
 *   0 <= _CVEC_DEF_AUTO_SK <= 1 */
#define _CVEC_GROWTH_FACTOR 2
#define _CVEC_DEF_CAP_MAN CVEC_CAPMAN_LOG
#define _CVEC_DEF_AUTO_SK 0

typedef unsigned char _cvector_byte;

/* VectorCapMan manages the way how the Vector Grows and Shrinks.
 * ## CVEC_CAPMAN_LOG ##
 *  - Growing rule: (size<=capacity && (capacity % _CVEC_GROWTH_FACTOR)==0)
 *  - Shrinking rule: (size<=capacity && size>=(capacity/_CVEC_GROWTH_FACTOR))
 * ## CVEC_CAPMAN_EQ_SZ ##
 *  - Growing rule: (size==capacity)
 *  - Shrinking rule: (size==capacity) */
typedef enum{CVEC_CAPMAN_LOG, CVEC_CAPMAN_EQ_SZ} VectorCapMan;

/* VectorData holds the data of a String, such as:
 * - Size and Capacity;
 * - Auto Shrink (on/off);
 * - Capacity Management;
 * - Auxiliary variable. */
typedef struct{
  size_t sz, cap, tp_sz;
  VectorCapMan capMan;
  char auto_shrink;
} VectorData;

/*----------------------------------Auxiliaries----------------------------------*/
/* Macro for Vector's type (int, double, float, boolean, etc...) */
#define Vector(type) type*

/* Verifies whether the Vector is valid */
char _vector_valid(Vector(void) *vec);
#define vector_valid(vec) (_vector_valid((Vector(void)*) vec))
/*===============================================================================*/

/*----------------------------------Constructor----------------------------------*/
/* Creates the vector and returns its address 
 * (pointer to array / pointer to pointer) */
Vector(void)* _vector_new(size_t tp_sz, VectorCapMan capMan, char auto_shrink);

#define vector_new_cfg(type, capMan, auto_shrink)                  \
  ((Vector(type)*) _vector_new(sizeof(type), capMan, auto_shrink))

#define vector_new(type) ((Vector(type)*) _vector_new(sizeof(type),\
  _CVEC_DEF_CAP_MAN, _CVEC_DEF_AUTO_SK))
/*==============================================================================*/

/*-----------------------------------Misc Info-----------------------------------*/
/* Returns a pointer to the VectorData Address
 * OBS:
 *  - This should not be used by the user, just by the API's functions.
 *    Incorrect use of this function can cause undefined behaviour.
 *  - The returned Pointer may become invalid if the actual address of
 *    VectorData* is modified by other functions (e.g., erase and insert) */
VectorData* __vector_data(Vector(void) *vec);
#define __vector_data_raw(vec) (((VectorData*)(*vec))-1)

/* Returns pointer to the beginning of Vector */
void* _vector_begin(Vector(void) *vec);
#define vector_begin(vec) (_vector_begin((Vector(void)*) vec))

/* Returns pointer to the end of Vector (after the last element) */
void* _vector_end(Vector(void) *vec);
#define vector_end(vec) (_vector_end((Vector(void)*) vec))

/* Returns Vector's Capacity Management */
VectorCapMan _vector_cap_man(Vector(void) *vec);
#define vector_cap_man(vec) (_vector_cap_man((Vector(void)*) vec))

/* Sets Vector's Capacity Management */
void _vector_set_cap_man(Vector(void) *vec, VectorCapMan capMan);
#define vector_set_cap_man(vec, capMan)(_vector_set_cap_man((Vector(void)*) vec, capMan))

/* Returns Vector's Type Size (tp_sz) */
size_t _vector_tp_sz(Vector(void) *vec);
#define vector_tp_sz(vec) (_vector_tp_sz((Vector(void)*) vec))

/* Returns Vector's auto_shrink */
char _vector_auto_shrink(Vector(void) *vec);
#define vector_auto_shrink(vec) (_vector_auto_shrink((Vector(void)*) vec))

/* Sets Vector's auto_shrink */
void _vector_set_auto_shrink(Vector(void) *vec, char auto_shrink);
#define vector_set_auto_shrink(vec, auto_shrink) (_vector_set_auto_shrink((Vector(void)*) vec, auto_shrink))
/*==============================================================================*/

/*----------------------------------Dimensions----------------------------------*/
/* Verifies whether the Vector is empty */
char _vector_empty(Vector(void) *vec);
#define vector_empty(vec) (_vector_empty((Vector(void)*) vec))

/* Returns Vector's Size */
size_t _vector_size(Vector(void) *vec);
#define vector_size(vec) (_vector_size((Vector(void)*) vec))

/* Sets Vector's Size
 * OBS: This shouldn't be used by the user, just by the API's functions.
 *      Instead, use insert and erase functions to increase and decrease
 *      Vector's size.
 *      Incorrect use of this function can cause undefined behaviour. */
void __vector_set_size(Vector(void) *vec, size_t new_size);

/* Returns Vector's Total Capacity */
size_t _vector_capacity(Vector(void) *vec);
#define vector_capacity(vec) (_vector_capacity((Vector(void)*) vec))

/* Sets Vector's Capacity
 * OBS: if new_cap < size, the Vector's size will be decreased */
void _vector_set_capacity(Vector(void) *vec, size_t new_cap);
#define vector_set_capacity(vec, new_cap) (_vector_set_capacity((Vector(void)*) vec,new_cap))

/* Requests the container to reduce its capacity to fit its size, based on
 * VectorCapMan of the Vector 'vec'*/
void _vector_shrink(Vector(void) *vec);
#define vector_shrink(vec) (_vector_shrink((Vector(void)*) vec))
/*==============================================================================*/

/*-----------------------------------Modifiers-----------------------------------*/
/* Sets an empty gap starting at a specific 'index' with a specific 'length' in the vector.
 * Returns: void pointer to the beginning of gap in the array of the String */
void* _vector_set_gap(Vector(void) *vec, size_t index, size_t length);
#define vector_set_gap(vec, index, length) (_vector_set_gap((Vector(void)*) vec, index, length))

/* OBS to all insert/append functions:
 *   1) No checks are performed to verify that the value of 'length' and 'index' of 'arr'.
 *      Sending an invalid value of any of these types will result in undefined behavior.
 *   2) No checks are performed to verify that the type size of inserted item matches with
 *      the Vector type size. All copy/inserting are done with the 'tp_sz' of the Vector.
 *      Sending values with different type sizes will result in undefined behavior.
 *   3) Return: Success -> Pointer to first element inserted in the vector
 *              Failure -> NULL */

/* Inserts 'length' elements of 'arr' inside 'vec' at a specific index */
void* _vector_insert_n(Vector(void) *vec, size_t index, void *arr, size_t length);
#define vector_insert_n(vec, index, arr, length)                      \
  (_vector_insert_n((Vector(void)*) vec, index, (void*) arr, length))

/* Inserts ALL elements of 'vec2' inside 'vec1' at a specific 'index' */
void* _vector_insert_all(Vector(void) *vec1, size_t index, Vector(void) *vec2);
#define vector_insert_all(vec1, index, vec2)                             \
  (_vector_insert_all((Vector(void)*) vec1, index, (Vector(void)*) vec2))

/* Inserts 1 element, by its 'ptr' address, inside 'vec' at a specific index */
#define vector_insert_one(vec, index, ptr)                       \
  (_vector_insert_n((Vector(void)*) vec, index, (void*) ptr, 1))

/* Appends 'length' elements of 'arr' at the end of 'vec' */
#define vector_append_n(vec, arr, length) (vector_insert_n(vec, vector_size(vec), arr, length))

/* Appends ALL elements of 'vec2' at the end of 'vec1' */
#define vector_append_all(vec1, vec2) (vector_insert_all(vec1, vector_size(vec), vec2))

/* Appends 1 element, by its 'ptr' address, at the end of 'vec' */
#define vector_append_one(vec, ptr) (vector_insert_one(vec, vector_size(vec), ptr))

/* OBS to all erase/pop functions:
 *   1) all erase requests with 'index' out of bounds will fail
 *   2) all erase requests with 'index' in bounds, but 'length' out of
 *      bounds will be executed just with the inbounds 'indexes'.
 *   3) Return: Success -> A pointer to the new location of the element that followed
 *              the last  element erased by the function call. This is the vector end if
 *              the operation erased the last element in the sequence.
 *              Failure -> NULL */

/* Removes a specific quantity of elements starting at a specific index from vector */
void* _vector_erase_n(Vector(void) *vec, size_t index, size_t length);
#define vector_erase_n(vec, index, len) (_vector_erase_n((Vector(void)*) vec, index, len))

/* Removes a single element from vector at specific index */
#define vector_erase(vec, index) (vector_erase_n(vec, index, 1))

/* Removes the last element in the vector, effectively reducing the container size by one. */
#define vector_pop_back(vec) (vector_erase_n(vec, vector_size(vec) - 1, 1))

/* Clears Vector's content */
#define vector_clear(vec) (__vector_set_size((Vector(void)*) vec, 0))

/* Frees all memory associated with the vector */
void _vector_free(Vector(void) *vec);
#define vector_free(vec) (_vector_free((Vector(void)*) vec))
/*================================================================================*/

#endif