/*
* Grupo nº: 25
* Afonso Santos - FC56368
* Alexandre Figueiredo - FC57099
* Raquel Domingos - FC56378
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "memory.h"
#include "memory-private.h"

void* create_shared_memory(char* name, int size) {
    int shm_fd, uid = getuid();
    void *ptr;
    int arrSize = stringSize(name) + numDigits(uid);
    char newName[arrSize];
    sprintf(newName, "%s%d", name, uid);
    shm_fd = shm_open(newName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("shm");
        exit(1);
    }
    int ret = ftruncate(shm_fd, size);
    if (ret == -1) {
        perror("shm");
        exit(2);
    }
    ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("shm-mmap");
        exit(3);
    }
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
        perror("/shm");
        exit(4);
    }
    ret = shm_unlink(newName);
    if (ret == -1) {
        perror("/shm");
        exit(8);
    }
}

void destroy_dynamic_memory(void* ptr) {
    free(ptr);
}

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
    op->id = -1;
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