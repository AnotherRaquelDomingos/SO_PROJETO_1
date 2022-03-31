#include <stdio.h>
#include "memory.h"
#include "main.h"

int execute_client(int client_id, struct communication_buffers* buffers, struct main_data* data) {
    return 1;
}

void client_process_operation(struct operation* op, int client_id, struct main_data* data, int* counter) {
    op->receiving_client = client_id;
    op->status = 'C';
    data->results[*counter] = *op;
    (*counter)++;
}

void client_get_operation(struct operation* op, int client_id, struct communication_buffers* buffers, struct main_data* data) {
    if (*(data->terminate) == 1) return;
    read_driver_client_buffer(buffers->driv_cli, client_id, data->max_ops, op);
}