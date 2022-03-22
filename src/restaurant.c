#include <stdio.h>
#include "memory.h"
#include "main.h"

void restaurant_process_operation(struct operation* op, int rest_id, struct main_data* data, int* counter) {
    op->receiving_rest = rest_id;
    op->status = 'R';
    data->results[*counter] = *op;
    *counter++;

    //TODO -> REVER O COUNTER E O ARRAY DE OPS (NO CLIENT.C E DRIVER.C TMB)
}

void restaurant_receive_operation(struct operation* op, int rest_id, struct communication_buffers* buffers, struct main_data* data) {
    
}