#pragma OPENCL EXTENSION cl_khr_fp64 : enable

#include "node.h"

__kernel void INITIAL(__global float *weight, __global int *visited, int N){
    int id = get_global_id(0);

    if (i >= N) return;

    weight[id] = INF;
    visited[id] = -1;
}

__kernel void FIND_MIN(__global float *weight, __global int *visited, __global float *g_min, __local float *l_min, int N){
    int id = get_global_id(0);
    int li = get_local_id(0);
    int localSize = get_local_size(0);

    l_min[li] = (id < N) ? weight[id] : INF;
    barrier(CLK_LOCAL_MEM_FENCE);

    for (int p = localSize / 2; p >= 1; p = p >> 1){
        if (li < p) l_min[li] = fmin(l_min[li], l_min[li + p]);
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if (li == 0){
        g_min[get_group_id(0)] = l_min[0];
    }
}

__kernel void RELAX(__global node *data, __global float *weight, __global int *visited, int N, int target, int MIN){
    int id = get_global_id(0);
    int tmp;
    if (id < N){
        float my_weight = weight[id];
        node my_node = data[id];
        if (my_weight <= MIN){
            for (tmp = 0; tmp < BRANCH; tmp++){                
                int next = my_node.adj_node[tmp].num;
                float f_next = my_weight + sqrt(pow(data[next].x - data[target].x, 2) + pow(data[next].y - data[target].y, 2));
                if (f_next <= weight[next]){
                    atomic_xchg(visited + next, id); // visited[next] = id;
                    atomic_xchg(weight + next, f_next); // weight[next] = f_next;
                }
            }
        }
    }
}

__kernel void EDGE_RELAX(__global node *data, __global float *weight, __global int *visited, int N, int target){
    int id = get_global_id(0);
    int tmp;
    if (id < N){
        float my_weight = weight[id];
        node my_node = data[id];
        if (my_weight < INF){
            for (tmp = 0; tmp < BRANCH; tmp++){
                int next = my_node.adj_node[tmp].num;
                float f_next = my_weight + sqrt(pow(data[next].x - data[target].x, 2) + pow(data[next].y - data[target].y, 2));
                if (next != -1 && f_next < weight[next]){
                    atomic_xchg(visited + next, id);
                    atomic_xchg(weight + next, f_next);
                }
            }
        }
    }
}

__kernel void SETTLE(__global node *data, __global float *weight, __global int *visited, int N){
    int id = get_global_id(0);
}