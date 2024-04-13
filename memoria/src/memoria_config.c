#include "memoria_config.h"

t_memoria_config* init_memoria_config(char* path){
    t_memoria_config* memoria_config = malloc(sizeof(t_memoria_config));

    memoria_config->config = config_create(path);
    if(memoria_config->config == NULL){
        printf("Error: No se pudo abrir el archivo de configuración\n");
        free(memoria_config);
        exit(EXIT_FAILURE);
    }

    memoria_config->ip_memoria = config_get_string_value(memoria_config->config, "IP_MEMORIA");
    memoria_config->puerto_escucha_memoria = config_get_string_value(memoria_config->config, "PUERTO_ESCUCHA");
    memoria_config->tam_memoria = config_get_int_value(memoria_config->config, "TAM_MEMORIA");
    memoria_config->tam_pagina = config_get_int_value(memoria_config->config, "TAM_PAGINA");
    memoria_config->path_instrucciones = config_get_string_value(memoria_config->config, "PATH_INSTRUCCIONES");
    memoria_config->retardo_respuesta = config_get_int_value(memoria_config->config, "RETARDO_RESPUESTA");

    return memoria_config;
}

void liberar_memoria_config(t_memoria_config* memoria_config) {
    config_destroy(memoria_config->config);
    
    free(memoria_config);
}
