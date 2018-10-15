#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "heap.h"
#include "vector.h"

#include <sys/time.h>
#include <unistd.h>

#define VMAX 500000
#define EMAX 1000000
#define INF 2100000000

double get_time(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (double)tv.tv_sec + (double)1e-6 * tv.tv_usec;
}

#define DATA "/home/jjun/opencl/astar/data/v500000_e1111111.csv"

typedef struct {
	int x, y;
}loc;

vector adj[VMAX+1];
float dist[VMAX+1];
loc location[VMAX+1];

float cal_h(loc u, loc v){
	float a = pow((float)u.x - v.x, 2);
	float b = pow((float)u.y - v.y, 2);
	float c = sqrt(a + b);
	return c;
}

int main(int argc, const char *argv[]){
    int n, m, start, end;
    int a, b, x, y;
    int w;
    heap pq;
    pair p;
    
	FILE* stream = fopen(DATA, "r");
	char line[1024];
    char *tmp;

	for (int i = 0; i<=VMAX; i++){
		init_vector(&adj[i]);
	}
	init_heap(&pq);

    // Input
    fgets(line, 1024, stream);
	tmp = strtok(line, ",");
	for (int i = 0; tmp != NULL; i++, tmp = strtok(NULL, ",")){
		if (i == 1)
			n = atoi(tmp);
		else if (i == 3)
			m = atoi(tmp);
	}

	printf("n : %d, m : %d\n", n, m);

    fgets(line, 1024, stream); // skip

	for (int i = 0; i<n; i++){
		fgets(line, 1024, stream);
		tmp = strtok(line, ",");
		for (int i = 0; tmp != NULL; i++, tmp = strtok(NULL, ",")){
			if (i == 0)
				x = atoi(tmp);
			else if (i == 1)
				y = atoi(tmp);
		}
//		printf("Node [%d] - (%d, %d)\n", i, x, y);

        location[i].x = x;
        location[i].y = y;
	}
	fgets(line, 1024, stream); // skip

	for (int i = 0; i<m; i++){
		fgets(line, 1024, stream);

		tmp = strtok(line, ",");
		for (int i = 0; tmp != NULL; i++, tmp = strtok(NULL, ",")){
			if (i == 0)
				a = atoi(tmp);
			else if (i == 1)
				b = atoi(tmp);
			else if (i == 2)
				w = atoi(tmp);
		}
//		printf("Edge [%d] - %d ~ %d(%f)\n", i, a, b, w);
		p.first = b;
		p.second = w;
		adj[a].push_back(&adj[a], p);
        /*for (int j = 0; j<BRANCH; j++){
            if (Node[b].adj_node[j].num == -1){
                Node[b].adj_node[j].num = a;
                Node[b].adj_node[j].weight = w;
                break;
            }
        }*/
	}

	fgets(line, 1024, stream);
	tmp = strtok(line, ",");
	for (int i = 0; tmp != NULL; i++, tmp = strtok(NULL, ",")){
		if (i == 1)
			start = atoi(tmp);
		else if (i == 3)
			end = atoi(tmp);
	}
	printf("Start : %d, End : %d\n", start, end);

	double start_time = get_time();

	for (int i = 0; i<n; i++){
		dist[i] = INF;
	}
	dist[start] = 0;

	p.first = start;
	p.second = dist[start];
	pq.push(&pq, p);
	while (pq.size(&pq)){
		p = pq.top(&pq);
		pq.pop(&pq);
		int u = p.first, cost = p.second;
		int len = adj[u].size;
		if (cost > dist[u]) continue;
		for (int i = 0; i<len; i++){
			pair* v = adj[u].accessByIndex(&adj[u], i);
			if (v->second + cost + cal_h(location[v->first], location[end]) < dist[v->first]){
				dist[v->first] = v->second + cost + cal_h(location[v->first], location[end]) ;
                p.first = v->first;
				p.second = dist[p.first];
                pq.push(&pq, p);
			}
		}
	}

	double end_time = get_time();
	double elapsed = end_time - start_time;

	printf("Total Weight : %f\n", dist[end]);

	printf("time : %lf\n", elapsed);

	FILE* fp_record = fopen("record_seq.txt", "a+");
	fprintf(fp_record, "[%d] time : %lf\n", n, elapsed);
	fclose(fp_record);

	return 0;
}
// + sqrt(pow(location[v->first].x - location[end].x, 2) + pow(location[v->first].y - location[end].y, 2)) 