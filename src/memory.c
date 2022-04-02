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
    shm_fd = shm_open(newName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    ftruncate(shm_fd, size);
    ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    return ptr;
}

void* create_dynamic_memory(int size) {
    void* ptr = malloc(size);
    bzero(ptr, size);
    return ptr;
}

void destroy_shared_memory(char* name, void* ptr, int size) {
    int uid = getuid();
    int arrSize = stringSize(name) + numDigits(uid);
    char newName[arrSize];
    sprintf(newName, "%s%d", name, uid);
    int ret = munmap(ptr, size);
    if (ret == -1) {
        perror(newName);
        exit(7);
    }
    ret = shm_unlink(newName);
    if (ret == -1) {
        perror(newName);
        exit(8);
    }
}

void destroy_dynamic_memory(void* ptr) {
    free(ptr);
}

//TODO verificar os se eh ponteiro ou conteudo do ponteiro
void write_main_rest_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    for (int i = 0 ; i < buffer_size ; i++) {
        if (buffer->ptrs[i] == 0) {
            buffer->ptrs[i] = 1;
            buffer->buffer[i] = *op;
            break; 
        }
    }   
}

void write_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {
    int in = buffer->ptrs->in;
    if (((in + 1) % buffer_size) != buffer->ptrs->out) {
        buffer->buffer[in] = *op;
        buffer->ptrs->in = (in + 1) % buffer_size;
    }
}

void write_driver_client_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
      for (int i = 0 ; i < buffer_size ; i++) {
        if (buffer->ptrs[i] == 0) {
            buffer->ptrs[i] = 1;
            buffer->buffer[i] = *op;
            break; 
        }
    } 
}

void read_main_rest_buffer(struct rnd_access_buffer* buffer, int rest_id, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        // int x = buffer->ptrs[i];
        // printf("chegooou\n"); 
      if (buffer->ptrs[i] == 1 && buffer->buffer[i].requested_rest == rest_id) {
          *op = buffer->buffer[i];
          buffer->ptrs[i] = 0;
          return;
      }  
    }
    op->id = -1;
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

void read_driver_client_buffer(struct rnd_access_buffer* buffer, int client_id, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 1 && buffer->buffer[i].requesting_client == client_id) {
            *op = buffer->buffer[i];
            buffer->ptrs[i] = 0;
            return;
        }
    }
    op->id = -1; // this line is read if the condtion above is never met
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