#define BRANCH 10
#define INF 2100000000

typedef struct adjust {
    int num;
    float weight;
}adjust;

typedef struct node {
    int x, y;
    adjust adj_node[BRANCH];
}node;