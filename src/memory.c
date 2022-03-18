#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

void* create_dynamic_memory(int size) {
    return malloc(size);
}

void destroy_dynamic_memory(void* ptr) {
    free(ptr);
}