#include <stdio.h>
#include "memory.h"
#include "main.h"

void driver_process_operation(struct operation* op, int driver_id, struct main_data* data, int* counter) {
    op->receiving_driver = driver_id;
    op->status = 'D';
    data->results[*counter] = *op;
    *counter++;
}