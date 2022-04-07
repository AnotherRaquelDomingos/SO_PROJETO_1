/*
* Grupo nº: 25
* Afonso Santos - FC56368
* Alexandre Figueiredo - FC57099
* Raquel Domingos - FC56378
*/
#include <stdio.h>
#include "memory.h"
#include "main.h"
#include "driver.h"

int execute_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data) {
    int processed_ops = 0;
    int *p_counter = &processed_ops;
    struct operation *op = create_dynamic_memory(sizeof(struct operation));
    while(1) {
        if (*(data->terminate) == 1) {
            destroy_dynamic_memory(op);
            return processed_ops;
        }
        driver_receive_operation(op, buffers, data);
        if (op->id != -1) {
            printf("Motorista recebeu pedido!\n");
            driver_process_operation(op, driver_id, data, p_counter);
            driver_send_answer(op, buffers, data);
        }
    }
}

void driver_receive_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data) {
    if (*(data->terminate) == 1) return;
    read_rest_driver_buffer(buffers->rest_driv, data->buffers_size, op);
}

void driver_process_operation(struct operation* op, int driver_id, struct main_data* data, int* counter) {
    op->receiving_driver = driver_id;
    op->status = 'D';
    data->results[op->id] = *op;
    (*counter)++;
}

void driver_send_answer(struct operation* op, struct communication_buffers* buffers, struct main_data* data) {
    write_driver_client_buffer(buffers->driv_cli, data->buffers_size, op);
}