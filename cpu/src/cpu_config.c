#include "cpu_config.h"

t_cpu_config* init_cpu_config(char* path){

    t_cpu_config* cpu_config = malloc(sizeof(t_cpu_config));

    cpu_config->config = config_create(path);

    if(cpu_config->config == NULL){
        printf("ERROR al abrir el .config\n");
        exit(EXIT_FAILURE);
    }

    cpu_config->ip_memoria = config_get_string_value(cpu_config->config, "IP_MEMORIA");
    cpu_config->puerto_memoria = config_get_string_value(cpu_config->config, "PUERTO_MEMORIA");
    cpu_config->ip_cpu = config_get_string_value(cpu_config->config, "IP_CPU");
    cpu_config->puerto_escucha_dispatch = config_get_string_value(cpu_config->config, "PUERTO_ESCUCHA_DISPATCH");
    cpu_config->puerto_escucha_interrupt = config_get_string_value(cpu_config->config, "PUERTO_ESCUCHA_INTERRUPT");
    cpu_config->cantidad_entradas_tlb = config_get_int_value(cpu_config->config, "CANTIDAD_ENTRADAS_TLB");
    cpu_config->algoritmo_tlb = config_get_string_value(cpu_config->config, "ALGORITMO_TLB");

    return cpu_config;
}

void liberar_cpu_config(t_cpu_config* cpu_config) {

    config_destroy(cpu_config->config);

    // no es necesario - son cadenas literales
    //free(cpu_config->ip_memoria);
    //free(cpu_config->puerto_memoria);
    //free(cpu_config->puerto_escucha_dispatch);
    //free(cpu_config->puerto_escucha_interrupt);
    //free(cpu_config->algoritmo_tlb);

    free(cpu_config);
}
