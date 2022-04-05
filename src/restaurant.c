#include <stdio.h>
#include "memory.h"
#include "main.h"
#include "restaurant.h"

int execute_restaurant(int rest_id, struct communication_buffers* buffers, struct main_data* data) {
    struct operation *op = create_dynamic_memory(sizeof(struct operation));
    int processed_ops = 0;
    int *p_counter = &processed_ops;
    while(1) {
        if (*(data->terminate) == 1) {
            printf("Flag ficou a 1\n");
            destroy_dynamic_memory(op);
            return processed_ops;
        }
        restaurant_receive_operation(op, rest_id, buffers, data);
        if (op->id != -1) {
            restaurant_process_operation(op, rest_id, data, p_counter);
            restaurant_forward_operation(op, buffers, data);
        }
    }
}

void restaurant_receive_operation(struct operation* op, int rest_id, struct communication_buffers* buffers, struct main_data* data) {
    if (*(data->terminate) == 1) return;
    read_main_rest_buffer(buffers->main_rest, rest_id, data->buffers_size, op);
}

void restaurant_process_operation(struct operation* op, int rest_id, struct main_data* data, int* counter) {
    op->receiving_rest = rest_id;
    op->status = 'R';
    data->results[*counter] = *op;
    (*counter)++;
}

void restaurant_forward_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data) {
    // data->restaurant_stats[(op->receiving_rest)-1]++;
    write_rest_driver_buffer(buffers->rest_driv, data->buffers_size, op);
}