#pragma OPENCL EXTENSION cl_khr_fp64 : enable

#include "node.h"

// Node Information
// Index means Node num
// Node.x , Node.y means location of Node
// Node.adj_node means directly connected to Node

// Other Information
// Weight array means real weight of Node
// open_list array means Node will be visited
// prev array means prev node of Node in path S to Node (S : start node)

// Static Analysis
// Time Complexity : O(d * (n' / p))
// when d : maximum depth of graph G
// n' : maximum number of nodes in depth i of graph G (i = 0, ..., d)
// p : number of Cores in GPU

// Required kernel function
// 1. Initialization
// 2. Edge-Relaxing

__kernel void INITIAL(__global float *weight, __global int *open_list, __global int *prev, int N){
    int id = get_global_id(0);

    if (id >= N) return;

    weight[id] = INF;
    open_list[id] = 0;
    prev[id] = -1;
}

__kernel void FIND_END(__global int *open_list, __global int *global_open, __local int *local_open, int N){
    int id = get_global_id(0);
    int li = get_local_id(0);
    int localSize = get_local_size(0);

    local_open[li] = (id < N) ? open_list[id] : 0;
    barrier(CLK_LOCAL_MEM_FENCE);

    for (int p = localSize / 2; p >= 1; p = p >> 1){
        if (li < p) local_open[li] = local_open[li] || local_open[li + p];
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if (li == 0){
        global_open[get_group_id(0)] = local_open[0];
    }
}

inline void atomic_test(volatile __global float *g_weight, float my_weight, volatile __global int *g_prev, int my_prev){
    float cur;
    while (my_weight < (cur = *g_weight)){
        my_weight = atomic_xchg(g_weight, min(cur, my_weight));
        my_prev = atomic_xchg(g_prev, my_prev);
    }
}

float cal_h(node u, node v){
    float a = pow((float)u.x - v.x, 2);
    float b = pow((float)u.y - v.y, 2);
    float c = sqrt(a + b);
    return c;
}

__kernel void EDGE_RELAXING(__global node *nodes, __global float *weight, __global int *open_list, __global int *prev, int N, int target){
    int id = get_global_id(0);
    int tmp;
    if (id < N){
        if (open_list[id] == 1){ // 1 means queued, 0 means not queued
            atomic_xchg(&open_list[id], 0);
            float my_weight = weight[id];
            node my_node = nodes[id];
            for (tmp = 0; tmp < BRANCH; tmp++){
                int next = my_node.adj_node[tmp].num;
                float next_weight = my_node.adj_node[tmp].weight;
                float f_next = my_weight + next_weight + cal_h(nodes[next], nodes[target]);// + sqrt(pow((float)(nodes[next].x - nodes[target].x), (float)2) + pow((float)(nodes[next].y - nodes[target].y), (float)2));
                if (next != -1 && f_next < weight[next]){
                    // 발생 가능 문제 : IF 문을 통과한 뒤 context-switching 이 발생해서 원치 않은 결과가 반영될 수 있음?
                    atomic_test(&weight[next], f_next, &prev[next], id);
                    //atomic_xchg(&prev[next], id);
                    atomic_xchg(&open_list[next], 1);
                }
            }
        }
    }
}

