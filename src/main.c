#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "memory.h"
#include "process.h"

int main(int argc, char *argv[]) {
    //init data structures
    struct main_data *data = create_dynamic_memory(sizeof(struct main_data));
    struct communication_buffers *buffers = create_dynamic_memory(sizeof(struct communication_buffers));
    buffers->main_rest = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->rest_driv = create_dynamic_memory(sizeof(struct circular_buffer));
    buffers->driv_cli = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    
    //execute main code
    main_args(argc, argv, data);
    create_dynamic_memory_buffers(data);
    create_shared_memory_buffers(data, buffers);
    launch_processes(buffers, data);
    user_interaction(buffers, data);
    
    //release memory before terminating
    destroy_dynamic_memory(data);
    destroy_dynamic_memory(buffers->main_rest);
    destroy_dynamic_memory(buffers->rest_driv);
    destroy_dynamic_memory(buffers->driv_cli);
    destroy_dynamic_memory(buffers);
    return 0;
}

void main_args(int argc, char* argv[], struct main_data *data) {
    data->max_ops = atoi(argv[1]);
    data->buffers_size = atoi(argv[2]);
    data->n_restaurants = atoi(argv[3]); 
    data->n_drivers = atoi(argv[4]);
    data->n_clients = atoi(argv[5]);
    // struct operation ops[data->max_ops];
    // data->results = ops;
    int rest_stats = 0, driver_stats = 0, client_stats = 0;
    data->restaurant_stats = &rest_stats;
    data->driver_stats = &driver_stats;
    data->client_stats = &client_stats;
}

void create_dynamic_memory_buffers(struct main_data* data) {
    data->restaurant_pids = create_dynamic_memory(data->n_restaurants); 
    data->driver_pids = create_dynamic_memory(data->n_drivers);
    data->client_pids = create_dynamic_memory(data->n_clients);
    data->restaurant_stats = create_dynamic_memory(data->n_restaurants);
    data->driver_stats = create_dynamic_memory(data->n_drivers);
    data->client_stats = create_dynamic_memory(data->n_clients);
}

//TODO ver se eh mesmo shareed ou dynamic
void create_shared_memory_buffers(struct main_data* data, struct communication_buffers* buffers) {
    buffers->main_rest->ptrs = create_shared_memory("/main_rest_ptrs", sizeof(int)*(data->buffers_size));
    buffers->main_rest->buffer = create_shared_memory("/main_rest_buffer", sizeof(struct operation)*(data->buffers_size));
    
    buffers->rest_driv->ptrs = create_shared_memory("/rest_driv_ptrs", sizeof(struct pointers)*(data->buffers_size));
    buffers->rest_driv->buffer = create_shared_memory("/rest_driv_buffer", sizeof(struct operation)*(data->buffers_size));

    buffers->driv_cli->ptrs = create_shared_memory("/driv_cli_ptrs", sizeof(int)*(data->buffers_size));
    buffers->driv_cli->buffer = create_shared_memory("/driv_cli_buffer", sizeof(struct operation)*(data->buffers_size));
    data->results = create_shared_memory("/results", sizeof(struct operation)*(data->max_ops));
    data->terminate = create_shared_memory("/terminate", sizeof(int));
}

void launch_processes(struct communication_buffers* buffers, struct main_data* data) {
    int x = data->n_restaurants;
    for(int i = 0; i < data->n_restaurants; i++) {
       data->restaurant_pids[i] = launch_restaurant((i+1), buffers, data);
    }
    for(int i = 0; i < data->n_drivers; i++) {
        data->driver_pids[i] = launch_driver((i+1), buffers, data);
    }
    for(int i = 0; i < data->n_clients; i++) {
        data->client_pids[i] = launch_client((i+1), buffers, data);
    }
}

void user_interaction(struct communication_buffers* buffers, struct main_data* data) {
    int option, counter = 0; 
    //TODO terminar
    do {
        printf("Insira o numero associado a escolha que pretende:\n");
        printf("1) Request <cliente> <restaurante> <dish>\n");
        printf("2) Status <id>\n");
        printf("3) Stop\n");
        printf("4) Help\n");
            scanf("%d", &option);
            switch(option) {
                case 1:
                    // scanf("%s", NULL);
                    create_request(&counter, buffers, data);
                    break;
                case 2:
                    read_status(data);
                    break;
                case 4: 
                    printf("1 (Request) - Cria um pedido de encomenda"
                        "do cliente <cliente> ao restaurante <restaurant>, pedindo o prato <dish>\n" 
                        "2 (Status) - Consulta o estado do pedido especificado em <id>\n"
                        "3 (Stop) - Termina a execucao do Sistema\n");
                        break;                          
            }
    } while (option != 3);
}

void create_request(int* op_counter, struct communication_buffers* buffers, struct main_data* data) {
    //Assumindo que op_counter comeca a 0
    if (*op_counter < data->max_ops) {
        struct operation *op = create_dynamic_memory(sizeof(struct operation));
        op->id = *(op_counter);
        int requested_rest, requesting_client;
        char dish[20];
        scanf("%d%d%s", &requesting_client, &requested_rest, dish);
        op->requested_rest = requested_rest;
        op->requesting_client = requesting_client;
        op->requested_dish = dish;
        write_main_rest_buffer(buffers->main_rest, data->buffers_size, op);       
        printf("O pedido #%d foi criado\n", (*op_counter)++);
    }
}

void read_status(struct main_data* data) {
    int id;
    printf("Insira o id do pedido do qual quer obter o status:\n");
    scanf("%d", &id);
    if (id >= 0 && id <= data->max_ops) { 
        printf("Id do cliente que fez o pedido: %d\n", data->results[id].requesting_client);
        printf("Id do restaurante requesitado: %d\n", data->results[id].requested_rest);
        // printf("Nome do prato pedido: %s\n", data->results[id].requested_dish);
        printf("Id do restaurante que recebeu e processou o pedido: %d\n", data->results[id].receiving_rest);
        printf("Id do motorista que recebeu e processou o pedido: %d\n", data->results[id].receiving_driver);
        printf("Id do cliente que recebeu o pedido: %d\n", data->results[id].receiving_client);
        printf("Onde vai o pedido: %c\n", data->results[id].status);
        printf("\n");
    }
    else {
        printf("Id invalido\n");
    } 
}

void wait_processes(struct main_data* data) {
    for (int i = 0; i < data->n_restaurants; i++) {
        wait_process(data->restaurant_pids[i]);
    }
    for (int i = 0; i < data->n_drivers; i++) {
        wait_process(data->driver_pids[i]);
    }
    for (int i = 0; i < data->n_clients; i++) {
        wait_process(data->client_pids[i]);
    }
}