#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define CHECK_ERROR(err) \
    if (err != CL_SUCCESS) {\
        printf("[%s:%d] OpenCL Error %d\n", __FILE__, __LINE__, err);\
        exit(EXIT_FAILURE);\
    }

double get_time(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)1e-6 * tv.tv_usec;
}

char *get_source_code(const char *file_name, size_t *len){
    char *source_code;
    size_t length;
    FILE *file = fopen(file_name, "r");
    if (file == NULL){
        printf("[%s:%d] Failed to open %s\n", __FILE__, __LINE__, file_name);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    length = (size_t)ftell(file);
    rewind(file);

    source_code = (char *)malloc(length + 1);
    fread(source_code, length, 1, file);
    source_code[length] = '\0';

    fclose(file);

    *len = length;
    return source_code;
}