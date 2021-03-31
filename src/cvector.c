#include <stdlib.h>
#include <string.h>
#include "cvector.h"

/*----------------------------------Auxiliaries----------------------------------*/
char _vector_valid(Vector(void) *vec){
  return (vec!=NULL && (*vec)!=NULL) ? 1 : 0;
}
/*==============================================================================*/

/*----------------------------------Constructor----------------------------------*/
Vector(void)* _vector_new(size_t tp_sz, VectorCapMan capMan, char auto_shrink){
  VectorData *vd = (VectorData*) malloc(sizeof(VectorData));
  Vector(void) *vec = (Vector(void)) malloc(sizeof(void*));
  *vec = (void*) (vd+1);

  vd->sz = vd->cap = 0;
  vd->tp_sz = tp_sz;
  vd->capMan = CVEC_CAPMAN_LOG<=capMan && capMan<=CVEC_CAPMAN_EQ_SZ ? capMan : _CVEC_DEF_CAP_MAN;
  vd->auto_shrink = auto_shrink ? 1 : 0;

  return vec;
}
/*==============================================================================*/

/*-----------------------------------Misc Info-----------------------------------*/
VectorData* __vector_data(Vector(void) *vec){
  return _vector_valid(vec) ? __vector_data_raw(vec) : NULL;
}

void* _vector_begin(Vector(void) *vec){
  return _vector_valid(vec) ? (*vec) : NULL;
}

void* _vector_end(Vector(void) *vec){
  VectorData *vd = __vector_data(vec);

  return vd
    ? (void*)((_cvector_byte*)(*vec) + vd->tp_sz * vd->sz)
    : NULL;
}

VectorCapMan _vector_cap_man(Vector(void) *vec){
  VectorData *vd = __vector_data(vec);
  return vd ? vd->capMan : _CVEC_DEF_CAP_MAN;
}

void _vector_set_cap_man(Vector(void) *vec, VectorCapMan capMan){
  VectorData *vd = __vector_data(vec);

  if(vd){
    vd->capMan = CVEC_CAPMAN_LOG<=capMan && capMan<=CVEC_CAPMAN_EQ_SZ ? capMan : _CVEC_DEF_CAP_MAN;
    if(vd->auto_shrink) _vector_shrink(vec);
  }
}

char _vector_auto_shrink(Vector(void) *vec){
  VectorData *vd = __vector_data(vec);
  return vd ? vd->auto_shrink : _CVEC_DEF_AUTO_SK;
}

void _vector_set_auto_shrink(Vector(void) *vec, char auto_shrink){
  VectorData *vd = __vector_data(vec);
  if(vd && (vd->auto_shrink = (auto_shrink ? 1 : 0)))
    _vector_shrink(vec);
}
/*==============================================================================*/

/*----------------------------------Dimensions----------------------------------*/
char _vector_empty(Vector(void) *vec){
  return _vector_size(vec)==0;
}

size_t _vector_size(Vector(void) *vec){
  VectorData *vd = __vector_data(vec);
  return vd ? vd->sz : (size_t)0;
}

void __vector_set_size(Vector(void) *vec, size_t new_size){
  VectorData *vd = __vector_data(vec);
  
  if(vd && new_size!=vd->sz){
    if(new_size > vd->cap){
      size_t new_cap;
      
      if(vd->capMan==CVEC_CAPMAN_LOG){
        new_cap = _CVEC_MAX(vd->cap, 1);
        while(new_size > new_cap) new_cap *= _CVEC_GROWTH_FACTOR;
      }
      else new_cap = new_size;

      _vector_set_capacity(vec, new_cap);
      __vector_data_raw(vec)->sz = new_size;
    }
    else{
      vd->sz = new_size;
      if(vd->auto_shrink)
        _vector_shrink(vec);
    }
  }
}

size_t _vector_capacity(Vector(void) *vec){
  VectorData *vd = __vector_data(vec);
  return vd ? vd->cap : (size_t)0;
}

void _vector_set_capacity(Vector(void) *vec, size_t new_cap){
  VectorData *vd = __vector_data(vec);

  if(vd && new_cap!=vd->cap){
    vd = realloc(vd, sizeof(VectorData) + vd->tp_sz * new_cap);
    (*vec) = (void*) (vd+1);
    vd->cap = new_cap; vd->sz = _CVEC_MIN(vd->cap, vd->sz);
  }
}

size_t _vector_tp_sz(Vector(void) *vec){
  VectorData *vd = __vector_data(vec);
  return vd ? vd->tp_sz : 0;
}

void _vector_shrink(Vector(void) *vec){
  VectorData *vd = __vector_data(vec);

  if(vd){
    size_t new_cap;

    if(vd->capMan==CVEC_CAPMAN_LOG){
      if(vd->sz==0) new_cap = 0;
      else{
        size_t next_cap = new_cap = vd->cap;
        while(vd->sz<=(next_cap/=_CVEC_GROWTH_FACTOR)) new_cap = next_cap;
      }
    }
    else new_cap = vd->sz;

    _vector_set_capacity(vec, new_cap);
  }
}
/*==============================================================================*/

/*-----------------------------------Modifiers-----------------------------------*/
void* _vector_set_gap(Vector(void) *vec, size_t index, size_t length){
  VectorData *vd = __vector_data(vec);

  if(vd && index<=vd->sz){
    size_t sz_left = index * vd->tp_sz, sz_right = (vd->sz - index) * vd->tp_sz,
      sz_block = length * vd->tp_sz;
    _cvector_byte *begin, *end;

    __vector_set_size(vec, vd->sz+length);

    begin = (_cvector_byte*) (*vec) + sz_left;
    end = begin + sz_block;

    memmove(end, begin, sz_right);

    return (void*) begin;
  }
  
  return NULL;
}

void* _vector_insert_n(Vector(void) *vec, size_t index, void *arr, size_t length){
  VectorData *vd = __vector_data(vec);

  if(vd && index<=vd->sz){
    size_t sz_block = length * vd->tp_sz;
    void *temp = (void*) malloc(sz_block), *dest;
    
    memcpy(temp, arr, sz_block);
    dest = _vector_set_gap(vec, index, length);
    memcpy(dest, temp, sz_block);
    
    free(temp);

    return dest;
  }

  return NULL;
}

void* _vector_insert_all(Vector(void) *vec1, size_t index, Vector(void) *vec2){
  VectorData *vd = __vector_data(vec2);
  return (vd) ? _vector_insert_n(vec1, index, (void*) (vd+1), vd->sz) : NULL;
}

void* _vector_erase_n(Vector(void) *vec, size_t index, size_t length){
  VectorData *vd = __vector_data(vec);

  if(vd && index<vd->sz){
    Vector(_cvector_byte) *bv = (Vector(_cvector_byte)*) vec;
    size_t sz_left = index * vd->tp_sz,
      sz_block = (length = _CVEC_MIN(length, vd->sz-index)) * vd->tp_sz,
      sz_right = (vd->sz - index - length) * vd->tp_sz;
    _cvector_byte *begin = (*bv) + sz_left, *end = begin + sz_block;

    memmove(begin, end, sz_right); __vector_set_size(vec, vd->sz-length);

    return (void*) ((*bv) + sz_left);
  }

  return NULL;
}

void _vector_free(Vector(void) *vec){
  free(__vector_data(vec)); free(vec);
}
/*================================================================================*/