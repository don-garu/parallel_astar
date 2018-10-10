//
//  main.c
//  graduateMission
//
//  Created by 손성호 on 2018. 9. 27..
//  Copyright © 2018년 Son Seong ho. All rights reserved.
//
//

#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
#include "vector.h"

#define VMAX 1000
#define EMAX 100000
#define INF 2100000000

vector adj[VMAX+1];
unsigned int dist[VMAX+1];

int main(int argc, const char * argv[]) {
    int v, e, s, d;
    heap pq;
    pair p;
    scanf("%d %d",&v, &e);
    
    // initialize vectors
    for(int i = 1; i <= v; i++){
        init_vector(&adj[i]);
    }
    
    // initialize heap
    init_heap(&pq);
    
    for(int i = 0; i < e; i++){
        int u, v, w;
        scanf("%d %d %d", &u, &v, &w);
        p.first = v;
        p.second = w;
        adj[u].push_back(&adj[u], p);
    }
    scanf("%d %d", &s, &d);
    
    for(int i = 1; i <= v; i++){
        dist[i] = INF;
    }
    
    dist[s] = 0;
    p.first = s;
    p.second = dist[s];
    pq.push(&pq, p);
    while(pq.size(&pq)){
        p = pq.top(&pq);
        pq.pop(&pq);
        int u = p.first, cost = p.second;
        int len = adj[u].size;
        if(cost > dist[u]) continue;
        for(int i = 0; i < len; i++){
            pair* v = adj[u].accessByIndex(&adj[u],i);
            if(v->second + cost < dist[v->first]){
                dist[v->first] = v->second + cost;
                p.first = v->first; p.second = dist[p.first];
                pq.push(&pq, p);
            }
        }
    }
    printf("%d\n",dist[d]);
    return 0;
}