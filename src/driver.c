#include <stdio.h>
#include "memory.h"
#include "main.h"

int execute_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data) {
    return 1;
}

void driver_process_operation(struct operation* op, int driver_id, struct main_data* data, int* counter) {
    op->receiving_driver = driver_id;
    op->status = 'D';
    data->results[*counter] = *op;
    (*counter)++;
}

void driver_receive_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data) {
    if (*(data->terminate) == 1) return;
    read_rest_driver_buffer(buffers->rest_driv, data->max_ops, op);
}

void driver_send_answer(struct operation* op, struct communication_buffers* buffers, struct main_data* data) {
    write_driver_client_buffer(buffers->driv_cli, data->max_ops, op);
}