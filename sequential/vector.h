//
//  vector.h
//  graduateMission
//
//  Created by 손성호 on 2018. 9. 28..
//  Copyright © 2018년 Son Seong ho. All rights reserved.
//

#ifndef vector_h
#define vector_h
#define INIT_CAPACITY 1024

// pair<int, int> typedef
typedef struct _pair{
    int first;
    int second;
} pair;

typedef struct _pair_vector{
    pair *_varr;
    unsigned int capacity;
    unsigned int size;
    pair* (*accessByIndex)(struct _pair_vector *_this, unsigned int index);
    pair (*back)(struct _pair_vector *_this);
    pair (*front)(struct _pair_vector *_this);
    void (*push_back)(struct _pair_vector *_this, pair node);
    void (*pop_back)(struct _pair_vector *_this);
    void (*clean)(struct _pair_vector *_this);
    int (*isEmpty)(struct _pair_vector *_this);
} vector;

void init_vector(vector *_this);
pair* accessByIndex(struct _pair_vector *_this, unsigned int index);
pair back(struct _pair_vector *_this);
pair front(struct _pair_vector *_this);
void push_back(struct _pair_vector *_this, pair node);
void pop_back(struct _pair_vector *_this);
void clean(struct _pair_vector *_this);
int isEmpty(struct _pair_vector *_this);

#endif /* vector_h */