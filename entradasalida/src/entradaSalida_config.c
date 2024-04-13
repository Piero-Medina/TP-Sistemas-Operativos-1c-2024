#include "entradaSalida_config.h"

t_entradaSalida_config* init_entradaSalida_config(char* path){
    t_entradaSalida_config* entradaSalida_config = malloc(sizeof(t_entradaSalida_config));

    entradaSalida_config->config = config_create(path);
    if(entradaSalida_config->config == NULL){
        printf("Error: No se pudo abrir el archivo de configuración\n");
        free(entradaSalida_config);
        exit(EXIT_FAILURE);
    }

    entradaSalida_config->tipo_interfaz = config_get_string_value(entradaSalida_config->config, "TIPO_INTERFAZ");
    entradaSalida_config->tiempo_unidad_trabajo = config_get_int_value(entradaSalida_config->config, "TIEMPO_UNIDAD_TRABAJO");
    entradaSalida_config->ip_kernel = config_get_string_value(entradaSalida_config->config, "IP_KERNEL");
    entradaSalida_config->puerto_kernel = config_get_string_value(entradaSalida_config->config, "PUERTO_KERNEL");
    entradaSalida_config->ip_memoria = config_get_string_value(entradaSalida_config->config, "IP_MEMORIA");
    entradaSalida_config->puerto_memoria = config_get_string_value(entradaSalida_config->config, "PUERTO_MEMORIA");
    entradaSalida_config->path_base_dialfs = config_get_string_value(entradaSalida_config->config, "PATH_BASE_DIALFS");
    entradaSalida_config->block_size = config_get_int_value(entradaSalida_config->config, "BLOCK_SIZE");
    entradaSalida_config->block_count = config_get_int_value(entradaSalida_config->config, "BLOCK_COUNT");

    return entradaSalida_config;
}

void liberar_entradaSalida_config(t_entradaSalida_config* entradaSalida_config) {
    config_destroy(entradaSalida_config->config);
    
    free(entradaSalida_config);
}
