#include <stdio.h>
#include "memory.h"
#include "main.h"

void client_process_operation(struct operation* op, int client_id, struct main_data* data, int* counter) {
    op->receiving_client = client_id;
    op->status = 'C';
    data->results[*counter] = *op;
    (*counter)++;
}