#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "memory.h"
#include "main.h"
#include "client.h"
#include "driver.h"
#include "restaurant.h"
#include "process.h"

int launch_restaurant(int restaurant_id, struct communication_buffers* buffers, struct main_data* data) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("process");
        exit(1);
    }
    else if (pid == 0) {
        exit(execute_restaurant(restaurant_id, buffers, data));
    }
    else {
        wait_process(pid);
        return pid;
    } 
}

int launch_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("process");
        exit(1);
    }
    else if (pid == 0) {
        exit(execute_driver(driver_id, buffers, data));
    }
    else {
        wait_process(pid);
        return pid;
    } 
}

int launch_client(int client_id, struct communication_buffers* buffers, struct main_data* data) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("process");
        exit(1);
    }
    else if (pid == 0) {
        exit(execute_client(client_id, buffers, data));
    }
    else {
        wait_process(pid);
        return pid;
    } 
}

int wait_process(int process_id) {
    int status;
    waitpid(process_id, &status, 0);
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    else {
        char error[30];
        sprintf(error, "Erro no processo %d", process_id);
        perror(error);
        exit(1);
    }
}

