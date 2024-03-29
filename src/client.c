/*
* Grupo nº: 25
* Afonso Santos - FC56368
* Alexandre Figueiredo - FC57099
* Raquel Domingos - FC56378
*/
#include <stdio.h>
#include "memory.h"
#include "main.h"
#include "client.h"

int execute_client(int client_id, struct communication_buffers* buffers, struct main_data* data) {
    int processed_ops = 0;
    int *p_counter = &processed_ops; // processed ops by this process
    struct operation *op = create_dynamic_memory(sizeof(struct operation));
    while(1) {
        if (*(data->terminate) == 1) {
            destroy_dynamic_memory(op);
            return processed_ops;
        }
        client_get_operation(op, client_id, buffers, data);
        if (op->id != -1) {
            printf("Cliente recebeu pedido!\n");
            client_process_operation(op, client_id, data, p_counter);
        }
    }
}

void client_get_operation(struct operation* op, int client_id, struct communication_buffers* buffers, struct main_data* data) {
    if (*(data->terminate) == 1) return;
    read_driver_client_buffer(buffers->driv_cli, client_id, data->buffers_size, op);
}

void client_process_operation(struct operation* op, int client_id, struct main_data* data, int* counter) {
    op->receiving_client = client_id;
    op->status = 'C';
    data->results[op->id] = *op;
    (*counter)++;
}