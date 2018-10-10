//
//  vector.c
//  graduateMission
//
//  Created by 손성호 on 2018. 9. 28..
//  Copyright © 2018년 Son Seong ho. All rights reserved.
//

#include "vector.h"
#include <stdlib.h>

void init_vector(vector *_this){
    _this->_varr = (pair*)malloc(sizeof(pair)*(INIT_CAPACITY+1));
    _this->size = 0;
    _this->capacity = INIT_CAPACITY;
    _this->accessByIndex = accessByIndex;
    _this->back = back;
    _this->front = front;
    _this->push_back = push_back;
    _this->pop_back = pop_back;
    _this->clean = clean;
    _this->isEmpty = isEmpty;
}

pair* accessByIndex(struct _pair_vector *_this, unsigned int index){
    return &(_this->_varr[index+1]);
}

pair back(struct _pair_vector *_this){
    return _this->_varr[_this->size];
}

pair front(struct _pair_vector *_this){
    return _this->_varr[1];
}

void push_back(struct _pair_vector *_this, pair node){
    pair *__varr = _this->_varr;
    if(_this->size + 1 > _this->capacity){
        unsigned int pcap = _this->capacity;
        pair *_ptmp;
        while((_ptmp = (pair*)realloc(__varr, sizeof(pair)*(pcap*2+1)))==NULL) ;
        _this->_varr = __varr = _ptmp;
        _this->capacity = 2*pcap;
    }
    __varr[++(_this->size)] = node;
}

void pop_back(struct _pair_vector *_this){
    if((--_this->size)*2 < _this->capacity){
        unsigned int pcap = _this->capacity;
        pair *_ptmp, *__varr = _this->_varr;
        while((_ptmp = (pair*)realloc(__varr, sizeof(pair)*(pcap/2+1)))==NULL) ;
        _this->_varr = _ptmp;
        _this->capacity = pcap/2;
    }
}

void clean(struct _pair_vector *_this){
    pair *_tmp, *__varr = _this->_varr;
    while((_tmp = (pair*)realloc(__varr, sizeof(pair)*(INIT_CAPACITY+1)))==NULL) ;
    _this->_varr = _tmp;
    _this->size = 0;
    _this->capacity = INIT_CAPACITY;
}

int isEmpty(struct _pair_vector *_this){
    return (_this->size == 0);
}