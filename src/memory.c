#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "memory.h"

int numDigits(int);
int stringSize(char*);

// int main() {
//     char *str = "string";
//     int uid = getuid();
//     char *uidStr;
//     int arrSize = stringSize(str) + numDigits(uid);
//     char newStr[100];
//     sprintf(newStr, "%s%d", str, uid);
//     // snprintf(newStr, 100, "%s%d", str, uid);
//     // strcat(str, uid);
//     printf("%ld\n", sizeof(str));
//     printf("%d\n", numDigits(uid));
//     printf("%d\n", arrSize);
//     printf("%s\n", newStr);
//     return 0;
// }

void* create_shared_memory(char* name, int size) {
    int shm_fd, uid = getuid();
    void *ptr;
    int arrSize = stringSize(name) + numDigits(uid);
    char newName[arrSize];
    sprintf(newName, "%s%d", name, uid);
    shm_fd = shm_open(newName, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, size);
    ptr = mmap(0, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    bzero(ptr, size);
    return ptr;
}

void* create_dynamic_memory(int size) {
    void* ptr = malloc(size);
    bzero(ptr, size);
    return ptr;
}

void destroy_dynamic_memory(void* ptr) {
    free(ptr);
}

void write_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {
    int in = buffer->ptrs->in;
    if (((in + 1) % buffer_size) != buffer->ptrs->out) {
        buffer->buffer[in] = op;
        buffer->ptrs->in = (in + 1) % buffer_size;
    }
}

void read_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {
    int out = buffer->ptrs->out;
    if (buffer->ptrs->in != out) {
        *op = buffer->buffer[out];
        buffer->ptrs->out = (out + 1) % buffer_size;
    }
    else {
        op->id = -1;
    }
}

int numDigits(int n) {
    int count = 0;
    do {
        n /= 10;
        count++;
    } while (n != 0);
    return count;
}


int stringSize(char* str) {
    int i = 0;
    for (i = 0; str[i] != '\0'; i++);
    return i;
}