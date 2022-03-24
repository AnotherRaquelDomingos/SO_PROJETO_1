#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "memory.h"

int main(int argc, char *argv[]) {
    //init data structures
    struct main_data *data = create_dynamic_memory(sizeof(struct main_data));
    struct communication_buffers *buffers = create_dynamic_memory(sizeof(struct communication_buffers));
    buffers->main_rest = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->rest_driv = create_dynamic_memory(sizeof(struct circular_buffer));
    buffers->driv_cli = create_dynamic_memory(sizeof(struct rnd_access_buffer));

    //execute main code
    main_args(argc, argv, data);
    printf("maxops:%d buffers:%d rests:%d driveers:%d clients:%d\n", data->max_ops, data->buffers_size, data->n_restaurants, data->n_drivers, data->n_clients);
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
    struct operation ops[data->max_ops];
    data->results = ops;
}

void user_interaction(struct communication_buffers* buffers, struct main_data* data) {
    int option;
    do {
    printf("Insira o numero associado a escolha que pretende:\n");
    printf("1) Request <cliente> <restaurante> <dish>\n");
    printf("2) Status <id>\n");
    printf("3) Stop\n");
    printf("4) Help\n");
        scanf("%d", &option);
        switch(option) {
            case 4: 
                printf("1 (Request) - Cria um pedido de encomenda"
                    "do cliente <cliente> ao restaurante <restaurant>, pedindo o prato <dish>\n" 
                    "2 (Status) - Consulta o estado do pedido especificado em <id>\n"
                    "3 (Stop) - Termina a execucao do Sistema\n");
                    break;                          
        }
    } while (option != 3);
}

void read_status(struct main_data* data) {
    int id, i;
    printf("Insira o id do pedido do qual quer obter o status: ");
    scanf("%d", &id);
    if (id >= 1 && id <= data->max_ops) { // Quando pede id = 1, vamos a posicao 0, etc...
        id--;
        printf("Id do cliente que fez o pedido: %d\n", data->results[id].requesting_client);
        printf("Id do restaurante requesitado: %d\n", data->results[id].requested_rest);
        printf("Nome do prato pedido: %s\n", data->results[id].requested_dish);
        printf("Id do restaurante que recebeu e processou o pedido: %d\n", data->results->receiving_rest);
        printf("Id do motorista que recebeu e processou o pedido: %d\n", data->results[id].receiving_driver);
        printf("Id do cliente que recebeu o pedido: %d\n", data->results[id].receiving_client);
    }
    else {
        printf("Id invalido\n");
    } 
}

void wait_processes(struct main_data* data) {
    for (int *restPointer = data->restaurant_pids; restPointer < data->restaurant_pids + data->n_restaurants; restPointer++) {
        waitpid(*restPointer,NULL,0);
    }
    for (int *drivPointer = data->driver_pids; drivPointer < data->driver_pids + data->n_drivers; drivPointer++) {
        waitpid(*drivPointer,NULL,0);
    }
    for (int *cliPointer = data->client_pids; cliPointer < data->client_pids + data->n_clients; cliPointer++) {
        waitpid(*cliPointer,NULL,0);
    }
}