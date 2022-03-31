#include <stdio.h>
#include "memory.h"
#include "main.h"

int execute_restaurant(int rest_id, struct communication_buffers* buffers, struct main_data* data) {
    return 1;
}

void restaurant_receive_operation(struct operation* op, int rest_id, struct communication_buffers* buffers, struct main_data* data) {
    if (*(data->terminate) == 1) return;
    read_main_rest_buffer(buffers->main_rest, rest_id, data->max_ops, op);
}

void restaurant_process_operation(struct operation* op, int rest_id, struct main_data* data, int* counter) {
    op->receiving_rest = rest_id;
    op->status = 'R';
    data->results[*counter] = *op;
    (*counter)++;
    //TODO -> REVER O COUNTER E O ARRAY DE OPS (NO CLIENT.C E DRIVER.C TMB)
}

void restaurant_forward_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data) {
    write_rest_driver_buffer(buffers->rest_driv, data->max_ops, op);
}