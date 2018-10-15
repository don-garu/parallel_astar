#include <CL/cl.h>
#include <sys/time.h>
#include <unistd.h>

//#define BRANCH 6

#include "node.h"
#include "lib.h"

#define KERNEL "kernel_v2.cl"
#define DATA "/home/jjun/opencl/astar/data/100000+/v200000_e310000.csv"
#define FACTOR 500

int main(int argc, char *argv[]){
    FILE* stream = fopen(DATA, "r");
    char line[1024];
    char *tmp;

    node *Node;
    int *open_list;
    int *prev;
    float *weight;
    int n, m;
    int x, y;
    int start, end;
    int a, b;
    float w;

    int *global_open;

    // Input
    fgets(line, 1024, stream);
    //printf("%s\n", line);
	tmp = strtok(line, ",");
	for (int i = 0; tmp != NULL; i++, tmp = strtok(NULL, ",")){
		if (i == 1)
			n = atoi(tmp);
		else if (i == 3)
			m = atoi(tmp);
	}

	printf("n : %d, m : %d\n", n, m);

    Node = (node*)calloc(sizeof(node), n);
    weight = (float*)calloc(sizeof(float), n);
    open_list = (int*)calloc(sizeof(int), n);
    prev = (int*)calloc(sizeof(int), n);

	fgets(line, 1024, stream); // skip
    //printf("%s\n", line);
	for (int i = 0; i<n; i++){
		fgets(line, 1024, stream);
        //printf("%s\n", line);
		tmp = strtok(line, ",");
		for (int i = 0; tmp != NULL; i++, tmp = strtok(NULL, ",")){
			if (i == 0)
				x = atoi(tmp);
			else if (i == 1)
				y = atoi(tmp);
		}
//		printf("Node [%d] - (%d, %d)\n", i, x, y);

        Node[i].x = x;
        Node[i].y = y;
        for (int j = 0 ;j<BRANCH; j++){
            Node[i].adj_node[j].num = -1;
        }
	}
	
	fgets(line, 1024, stream); // skip
    //printf("%s\n", line);

	for (int i = 0; i<m; i++){
		fgets(line, 1024, stream);
        //printf("%s\n", line);
		tmp = strtok(line, ",");
		for (int i = 0; tmp != NULL; i++, tmp = strtok(NULL, ",")){
			if (i == 0)
				a = atoi(tmp);
			else if (i == 1)
				b = atoi(tmp);
			else if (i == 2)
				w = atof(tmp);
		}
//		printf("Edge [%d] - %d ~ %d(%f)\n", i, a, b, w);

        for (int j = 0; j<BRANCH; j++){
            if (Node[a].adj_node[j].num == -1){
                Node[a].adj_node[j].num = b;
                Node[a].adj_node[j].weight = w;
                break;
            }
        }
        /*for (int j = 0; j<BRANCH; j++){
            if (Node[b].adj_node[j].num == -1){
                Node[b].adj_node[j].num = a;
                Node[b].adj_node[j].weight = w;
                break;
            }
        }*/
	}

	fgets(line, 1024, stream);
    //printf("1%s\n", line);
	tmp = strtok(line, ",");
	for (int i = 0; tmp != NULL; i++, tmp = strtok(NULL, ",")){
		if (i == 1)
			start = atoi(tmp);
		else if (i == 3)
			end = atoi(tmp);
	}
	printf("Start : %d, End : %d\n", start, end);

/*    for (int i = 0; i<n; i++){
        printf("Node[%d] loc : (%d, %d)\n", i, Node[i].x, Node[i].y);
        for (int j = 0; j<BRANCH; j++){
            if (Node[i].adj_node[j].num != -1){
                printf("NODE[%d] : NODE[%d], weight : %f\n", i, Node[i].adj_node[j].num, Node[i].adj_node[j].weight);
            }
        }
    }*/

    cl_platform_id platform;
    cl_uint num_platforms;
    cl_device_id device;
    cl_uint num_devices;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel *kernel;
    cl_int err;

    char *kernel_source;
    size_t kernel_source_size;

    size_t global_size = n;
    size_t local_size = FACTOR;
    size_t group_size = global_size / local_size;

    err = clGetPlatformIDs(1, &platform, &num_platforms);
    CHECK_ERROR(err);

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &num_devices);
    CHECK_ERROR(err);

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    CHECK_ERROR(err);

    queue = clCreateCommandQueue(context, device, 0, &err);
    CHECK_ERROR(err);

    kernel_source = get_source_code(KERNEL, &kernel_source_size);
    program = clCreateProgramWithSource(context, 1, (const char**)&kernel_source, &kernel_source_size, &err);
    CHECK_ERROR(err);

    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (err == CL_BUILD_PROGRAM_FAILURE){
        size_t log_size;
        char *log;

        err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        CHECK_ERROR(err);

        log = (char*)malloc(log_size + 1);
        err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        CHECK_ERROR(err);

        log[log_size] = '\0';
        printf("Compiler Error:\n%s\n", log);
        free(log);
    }
    CHECK_ERROR(err);

    kernel = (cl_kernel*)calloc(sizeof(cl_kernel), 3);

    kernel[0] = clCreateKernel(program, "INITIAL", &err);
    CHECK_ERROR(err);
    
    kernel[1] = clCreateKernel(program, "FIND_END", &err);
    CHECK_ERROR(err);

    kernel[2] = clCreateKernel(program, "EDGE_RELAXING", &err);
    CHECK_ERROR(err);

    cl_mem buf_node, buf_visited, buf_weight, buf_prev, buf_gopen, buf_open_list;

    buf_node = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(node), NULL, &err);
    CHECK_ERROR(err);
    buf_open_list = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(int), NULL, &err);
    CHECK_ERROR(err);
    buf_weight = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(float), NULL, &err);
    CHECK_ERROR(err);
    buf_prev = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(int), NULL, &err);
    CHECK_ERROR(err);
    buf_gopen = clCreateBuffer(context, CL_MEM_READ_WRITE, group_size * sizeof(int), NULL, &err);
    CHECK_ERROR(err);

    err = clEnqueueWriteBuffer(queue, buf_node, CL_FALSE, 0, n * sizeof(node), Node, 0, NULL, NULL);
    CHECK_ERROR(err);
    err = clEnqueueWriteBuffer(queue, buf_open_list, CL_FALSE, 0, n * sizeof(int), open_list, 0, NULL, NULL);
    CHECK_ERROR(err);
    err = clEnqueueWriteBuffer(queue, buf_prev, CL_FALSE, 0, n * sizeof(int), prev, 0, NULL, NULL);
    CHECK_ERROR(err);
    err = clEnqueueWriteBuffer(queue, buf_weight, CL_FALSE, 0, n * sizeof(float), weight, 0, NULL, NULL);
    CHECK_ERROR(err);

    global_open = (int*)calloc(sizeof(int), group_size);
    err = clEnqueueWriteBuffer(queue, buf_gopen, CL_TRUE, 0, group_size * sizeof(int), global_open, 0, NULL, NULL);
    CHECK_ERROR(err);

    double start_time = get_time();
    // STEP 1
    // INITIAL
    err = clSetKernelArg(kernel[0], 0, sizeof(cl_mem), &buf_weight);
    CHECK_ERROR(err);
    err = clSetKernelArg(kernel[0], 1, sizeof(cl_mem), &buf_open_list);
    CHECK_ERROR(err);
    err = clSetKernelArg(kernel[0], 2, sizeof(cl_mem), &buf_prev);
    CHECK_ERROR(err);
    err = clSetKernelArg(kernel[0], 3, sizeof(int), &n);
    CHECK_ERROR(err);

    err = clEnqueueNDRangeKernel(queue, kernel[0], 1, NULL, &global_size, &local_size, 0, NULL, NULL);
    CHECK_ERROR(err);

    err = clFinish(queue);
    CHECK_ERROR(err);

    err = clEnqueueReadBuffer(queue, buf_open_list, CL_FALSE, 0, sizeof(int) * n, open_list, 0, NULL, NULL);
    CHECK_ERROR(err);
    err = clEnqueueReadBuffer(queue, buf_weight, CL_TRUE, 0, sizeof(float) * n, weight, 0, NULL, NULL);
    CHECK_ERROR(err);

    // STEP 2
    weight[start] = 0;
    open_list[start] = 1;

    err = clEnqueueWriteBuffer(queue, buf_weight, CL_FALSE, 0, sizeof(float) * n, weight, 0, NULL, NULL);
    CHECK_ERROR(err);
    err = clEnqueueWriteBuffer(queue, buf_open_list, CL_TRUE, 0, sizeof(int) * n, open_list, 0, NULL, NULL);
    CHECK_ERROR(err);

    int flag = 0;
    // STEP 3
    while (1){
/*      printf("weight\n");
        for (int i = 0; i<n; i++){
            printf("%f ", weight[i]);
        }
        printf("\nopen_list\n");
        for (int i = 0; i<n; i++){
            printf("%d ", open_list[i]);
        }*/
        // FIND_END
        err = clSetKernelArg(kernel[1], 0, sizeof(cl_mem), &buf_open_list);
        CHECK_ERROR(err);
        err = clSetKernelArg(kernel[1], 1, sizeof(cl_mem), &buf_gopen);
        CHECK_ERROR(err);
        err = clSetKernelArg(kernel[1], 2, sizeof(int) * local_size, NULL);
        CHECK_ERROR(err);
        err = clSetKernelArg(kernel[1], 3, sizeof(int), &n);
        CHECK_ERROR(err);

        err = clEnqueueNDRangeKernel(queue, kernel[1], 1, NULL, &global_size, &local_size, 0, NULL, NULL);
        CHECK_ERROR(err);

        err = clFinish(queue);
        CHECK_ERROR(err);

        err = clEnqueueReadBuffer(queue, buf_gopen, CL_TRUE, 0, sizeof(int) * group_size, global_open, 0, NULL, NULL);
        CHECK_ERROR(err);

        flag = 0;
        for (int i = 0; i<group_size; i++){
            if (global_open[i] == 1)
                flag = 1;
        }
        if (flag == 0){
            break;
        }

        // EDGE_RELAXING       
        
        err = clSetKernelArg(kernel[2], 0, sizeof(cl_mem), &buf_node);
        CHECK_ERROR(err);
        err = clSetKernelArg(kernel[2], 1, sizeof(cl_mem), &buf_weight);
        CHECK_ERROR(err);
        err = clSetKernelArg(kernel[2], 2, sizeof(cl_mem), &buf_open_list);
        CHECK_ERROR(err);
        err = clSetKernelArg(kernel[2], 3, sizeof(cl_mem), &buf_prev);
        CHECK_ERROR(err);
        err = clSetKernelArg(kernel[2], 4, sizeof(int), &n);
        CHECK_ERROR(err);
        err = clSetKernelArg(kernel[2], 5, sizeof(int), &end);
        CHECK_ERROR(err);

        err = clEnqueueNDRangeKernel(queue, kernel[2], 1, NULL, &global_size, &local_size, 0, NULL, NULL);
        CHECK_ERROR(err);

        err = clFinish(queue);
        CHECK_ERROR(err);

        err = clEnqueueReadBuffer(queue, buf_open_list, CL_TRUE, 0, sizeof(int) * n, open_list, 0, NULL, NULL);
        CHECK_ERROR(err);
    }

    err = clEnqueueReadBuffer(queue, buf_weight, CL_FALSE, 0, sizeof(float) * n, weight, 0, NULL, NULL);
    CHECK_ERROR(err);
    err = clEnqueueReadBuffer(queue, buf_prev, CL_TRUE, 0, sizeof(int) * n, prev, 0, NULL, NULL);
    CHECK_ERROR(err);

    double end_time = get_time();
    double elapsed = end_time - start_time;

    printf("Total Weight : %f\n", weight[end]);
    int t = end;
    printf("%d", t);
    while (prev[t] != -1){
        printf(" - %d", prev[t]);
        t = prev[t];
    }
    printf("\n");

    printf("time : %lf\n", elapsed);

    return 0;
}