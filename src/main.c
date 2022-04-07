/*
* Grupo nº: 25
* Afonso Santos - FC56368
* Alexandre Figueiredo - FC57099
* Raquel Domingos - FC56378
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
}

void create_dynamic_memory_buffers(struct main_data* data) {
    data->restaurant_pids = create_dynamic_memory(sizeof(int)*(data->n_restaurants)); 
    data->driver_pids = create_dynamic_memory(sizeof(int)*(data->n_drivers));
    data->client_pids = create_dynamic_memory(sizeof(int)*(data->n_clients));
    data->restaurant_stats = create_dynamic_memory(sizeof(int)*(data->n_restaurants));
    data->driver_stats = create_dynamic_memory(sizeof(int)*(data->n_drivers));
    data->client_stats = create_dynamic_memory(sizeof(int)*(data->n_clients));
}

void create_shared_memory_buffers(struct main_data* data, struct communication_buffers* buffers) {
    buffers->main_rest->ptrs = create_shared_memory(STR_SHM_MAIN_REST_PTR, sizeof(int)*(data->buffers_size));
    buffers->main_rest->buffer = create_shared_memory(STR_SHM_MAIN_REST_BUFFER, sizeof(struct operation)*(data->buffers_size));
    
    buffers->rest_driv->ptrs = create_shared_memory(STR_SHM_REST_DRIVER_PTR , sizeof(struct pointers)*(data->buffers_size));
    buffers->rest_driv->buffer = create_shared_memory(STR_SHM_REST_DRIVER_BUFFER, sizeof(struct operation)*(data->buffers_size));

    buffers->driv_cli->ptrs = create_shared_memory(STR_SHM_DRIVER_CLIENT_PTR , sizeof(int)*(data->buffers_size));
    buffers->driv_cli->buffer = create_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER, sizeof(struct operation)*(data->buffers_size));
    
    data->results = create_shared_memory(STR_SHM_RESULTS, sizeof(struct operation)*(data->max_ops));
    data->terminate = create_shared_memory(STR_SHM_TERMINATE, sizeof(int));
}

void launch_processes(struct communication_buffers* buffers, struct main_data* data) {
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
    char option[7];
    int  counter = 0; 
    //TODO terminar
    printf("Insira o numero associado a escolha que pretende:\n");
    printf("\trequest <cliente> <restaurante> <dish>\n");
    printf("\tstatus <id>\n");
    printf("\tstop\n");
    printf("\thelp\n");
    do {
        printf("\nInsira acao :\n");
        scanf("%s", option);
        if(strcmp(option,"request") == 0){
                create_request(&counter, buffers, data);
                counter++;
        }
        else if(strcmp(option,"status") == 0){
                read_status(data);
        }
        else if(strcmp(option,"stop")== 0){
                stop_execution(data, buffers);
        }
        else if(strcmp(option,"help")== 0){
                printf("request - Cria um pedido de encomenda"
                    "do cliente <cliente> ao restaurante <restaurant>, pedindo o prato <dish>\n" 
                    "status - Consulta o estado do pedido especificado em <id>\n"
                    "stop - Termina a execucao do Sistema\n");
        }
    } while (strcmp(option,"stop")!= 0);
}
                 
void create_request(int* op_counter, struct communication_buffers* buffers, struct main_data* data) {
    //Assumindo que op_counter comeca a 0
    if (*op_counter < data->max_ops) {
        struct operation *op = create_dynamic_memory(sizeof(struct operation));
        op->id = *(op_counter);
        op->status = 'I';
        int requested_rest, requesting_client;
        char *dish = create_dynamic_memory(20);
        scanf("%d%d%s", &requesting_client, &requested_rest, dish);
        op->requested_rest = requested_rest;
        op->requesting_client = requesting_client;
        op->requested_dish = dish;
        data->results[*op_counter] = *op;
        write_main_rest_buffer(buffers->main_rest, data->buffers_size, op);       
        destroy_dynamic_memory(op); 
        printf("O pedido #%d foi criado\n", *op_counter);
    }
}

void read_status(struct main_data* data) {
    int id;
    scanf("%d", &id);
    if (id >= 0 && data->results[id].requested_dish != NULL) { 
        printf("Id do cliente que fez o pedido: %d\n", data->results[id].requesting_client);
        printf("Id do restaurante requesitado: %d\n", data->results[id].requested_rest);
        printf("Nome do prato pedido: %s\n", data->results[id].requested_dish);
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

void stop_execution(struct main_data* data, struct communication_buffers* buffers) {
    *(data->terminate) = 1;
    wait_processes(data);
    write_statistics(data);
    destroy_memory_buffers(data, buffers);
}

void wait_processes(struct main_data* data) {
    for (int i = 0; i < data->n_restaurants; i++) {
        data->restaurant_stats[i] = wait_process(data->restaurant_pids[i]);
    }
    for (int i = 0; i < data->n_drivers; i++) {
        data->driver_stats[i] = wait_process(data->driver_pids[i]);
    }
    for (int i = 0; i < data->n_clients; i++) {
        data->client_stats[i] = wait_process(data->client_pids[i]);
    }
}

void write_statistics(struct main_data* data) {
    printf("Terminando o MagnaEats. Imprimindo estatisticas:\n");
    for (int i = 0; i < data->n_restaurants; i++) {
        printf("O restaurante %d preparou %d pedidos.\n", i+1, data->restaurant_stats[i]);
    }
    for (int i = 0; i < data->n_drivers; i++) {
        printf("O motorista %d entregou %d pedidos.\n", i+1, data->driver_stats[i]);
    }
    for (int i = 0; i < data->n_clients; i++) {
        printf("O cliente %d recebeu %d pedidos.\n", i+1, data->client_stats[i]);
    }
}

void destroy_memory_buffers(struct main_data* data, struct communication_buffers* buffers) {

    for (int i = 0; data->results[i].requested_dish != NULL; i++) {
        destroy_dynamic_memory(data->results[i].requested_dish);
    }
    
    destroy_dynamic_memory(data->restaurant_pids);
    destroy_dynamic_memory(data->driver_pids);
    destroy_dynamic_memory(data->client_pids);
    destroy_dynamic_memory(data->restaurant_stats);
    destroy_dynamic_memory(data->driver_stats);
    destroy_dynamic_memory(data->client_stats);
    
    destroy_shared_memory(STR_SHM_RESULTS, data->results, sizeof(struct operation)*(data->max_ops));
    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, sizeof(int));

    destroy_shared_memory(STR_SHM_MAIN_REST_PTR, buffers->main_rest->ptrs, sizeof(int)*(data->buffers_size));
    destroy_shared_memory(STR_SHM_MAIN_REST_BUFFER, buffers->main_rest->buffer, sizeof(struct operation)*(data->buffers_size));

    destroy_shared_memory(STR_SHM_REST_DRIVER_PTR, buffers->rest_driv->ptrs, sizeof(struct pointers)*(data->buffers_size));
    destroy_shared_memory(STR_SHM_REST_DRIVER_BUFFER, buffers->rest_driv->buffer, sizeof(struct operation)*(data->buffers_size));

    destroy_shared_memory(STR_SHM_DRIVER_CLIENT_PTR, buffers->driv_cli->ptrs, sizeof(int)*(data->buffers_size));
    destroy_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER, buffers->driv_cli->buffer, sizeof(struct operation)*(data->buffers_size));
}