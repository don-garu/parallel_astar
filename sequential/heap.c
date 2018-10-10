//
//  heap.c
//  graduateMission
//
//  Created by 손성호 on 2018. 9. 27..
//  Copyright © 2018년 Son Seong ho. All rights reserved.
//

#include "heap.h"
#include "vector.h"
#include <stdlib.h>

void init_heap(heap *_this){
    init_vector(&(_this->_harr));
    _this->capacity = _h_capacity;
    _this->size = _h_size;
    _this->top = _h_top;
    _this->push = _h_push;
    _this->pop = _h_pop;
    _this->clean = _h_clean;
    _this->isEmpty = _h_isEmpty;
    _this->comp = NULL;
    _this->setComp = _h_setComp;
}

unsigned int _h_capacity(struct _heap *_this){
    return _this->_harr.capacity;
}
unsigned int _h_size(struct _heap *_this){
    return _this->_harr.size;
}

pair _h_top(heap *_this){
    return (_this->_harr).front(&(_this->_harr));
}

void _h_push(heap *_this, pair node){
    int index;
    vector *__harr = &(_this->_harr);
    __harr->push_back(__harr, node);
    
    index = _this->size(_this);
    while(index > 1){
        pair* cur = (__harr->accessByIndex(__harr, index-1)), *parent = (__harr->accessByIndex(__harr, index/2-1));
        if((_this->comp == NULL && cur->first < parent->first) || \
           (_this->comp != NULL && _this->comp(cur, parent) < 0)){
            if(cur->first < parent->first){
                pair tmp = *cur;
                *cur = *parent;
                *parent = tmp;
                index/=2;
            }
        }else break;
    }
}

void _h_pop(heap *_this){
    int index = 1, tmpindex;
    vector *__harr = &(_this->_harr);
    pair tmp = __harr->back(__harr);
    int psize = __harr->size;
    __harr->pop_back(__harr);
    if(__harr->size == 0) return;
    
    *(__harr->accessByIndex(__harr, 0)) = tmp;
    while(index*2 < psize){
        pair* left = (__harr->accessByIndex(__harr, index*2-1)), *right = (__harr->accessByIndex(__harr, index*2));
        pair *cmp, *cur;
        if(index*2+1 < psize){
            if(_this->comp == NULL){
                if(left->first < right->first) tmpindex = index*2;
                else tmpindex = index*2+1;
            }else{
                if(_this->comp(left, right) < 0) tmpindex = index*2;
                else tmpindex = index*2+1;
            }
        }else{
            tmpindex = index*2;
        }
        cmp = (__harr->accessByIndex(__harr, tmpindex-1));
        cur = (__harr->accessByIndex(__harr, index-1));
        if((_this->comp == NULL && cmp->first < cur->first) || \
           (_this->comp != NULL && _this->comp(cmp, cur) < 0)){
            tmp = *cmp;
            *cmp = *cur;
            *cur = tmp;
            index = tmpindex;
        }else break;
    }
}

void _h_clean(heap *_this){
    (_this->_harr).clean(&(_this->_harr));
}

int _h_isEmpty(heap *_this){
    return ((_this->size(_this)) == 0);
}

void _h_setComp(heap *_this, int (*comp)(pair *a, pair *b)){
    _this->comp = comp;
}