#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "memory.h"

typedef struct {
    int nmb;
    int mb;
}Num;

int main() {
    Num *n = create_shared_memory("/cena", 1025);
    Num n0;
    n0.nmb = 1;
    n0.mb = 2;
    n[7] = n0;
    printf("%d\n",n[7].nmb);
    // destroy_shared_memory("/cena", n, 1);
//     int n[] = {1,2,3};
//     printf("%ld",sizeof(*n));
}