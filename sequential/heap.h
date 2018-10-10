//
//  heap.h
//  graduateMission
//
//  Created by 손성호 on 2018. 9. 27..
//  Copyright © 2018년 Son Seong ho. All rights reserved.
//

#ifndef heap_h
#define heap_h
#include "vector.h"

typedef struct _heap{
    vector _harr;
    unsigned int (*capacity)(struct _heap *_this);
    unsigned int (*size)(struct _heap *_this);
    pair (*top)(struct _heap *_this);
    void (*push)(struct _heap *_this, pair node);
    void (*pop)(struct _heap *_this);
    void (*clean)(struct _heap *_this);
    int (*isEmpty)(struct _heap *_this);
    int (*comp)(pair *a, pair *b);
    void (*setComp)(struct _heap *_this, int (*comp)(pair *a, pair *b));
} heap;

void init_heap(heap *_this);
unsigned int _h_capacity(struct _heap *_this);
unsigned int _h_size(struct _heap *_this);
pair _h_top(heap *_this);
void _h_push(heap *_this, pair node);
void _h_pop(heap *_this);
void _h_clean(heap *_this);
int _h_isEmpty(heap *_this);
void _h_setComp(heap *_this, int (*comp)(pair *a, pair *b));

#endif /* heap_h */