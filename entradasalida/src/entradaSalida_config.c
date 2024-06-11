#include "entradaSalida_config.h"

t_entradaSalida_config* init_entradaSalida_config(char* path, tipo_interfaz tipo_interfaz){
    t_entradaSalida_config* entradaSalida_config = malloc(sizeof(t_entradaSalida_config));

    entradaSalida_config->config = config_create(path);
    if(entradaSalida_config->config == NULL){
        printf("Error: No se pudo abrir el archivo de configuración\n");
        free(entradaSalida_config);
        exit(EXIT_FAILURE);
    }

    entradaSalida_config->tipo_interfaz = config_get_string_value(entradaSalida_config->config, "TIPO_INTERFAZ");

    if(!(tipo_interfaz == STDIN))
    entradaSalida_config->tiempo_unidad_trabajo = config_get_int_value(entradaSalida_config->config, "TIEMPO_UNIDAD_TRABAJO");

    entradaSalida_config->ip_kernel = config_get_string_value(entradaSalida_config->config, "IP_KERNEL");
    entradaSalida_config->puerto_kernel = config_get_string_value(entradaSalida_config->config, "PUERTO_KERNEL");

    if(!(tipo_interfaz == GENERICA)){
        entradaSalida_config->ip_memoria = config_get_string_value(entradaSalida_config->config, "IP_MEMORIA");
        entradaSalida_config->puerto_memoria = config_get_string_value(entradaSalida_config->config, "PUERTO_MEMORIA");


        if(tipo_interfaz == DIALFS){
            entradaSalida_config->path_base_dialfs = config_get_string_value(entradaSalida_config->config, "PATH_BASE_DIALFS");
            entradaSalida_config->block_size = config_get_int_value(entradaSalida_config->config, "BLOCK_SIZE");
            entradaSalida_config->block_count = config_get_int_value(entradaSalida_config->config, "BLOCK_COUNT");
            entradaSalida_config->retraso_compactacion = config_get_int_value(entradaSalida_config->config, "RETRASO_COMPACTACION");
        }
    }

    return entradaSalida_config;
}

void liberar_entradaSalida_config(t_entradaSalida_config* entradaSalida_config) {
    config_destroy(entradaSalida_config->config);
    
    free(entradaSalida_config);
}

void imprimir_config(t_entradaSalida_config* config, tipo_interfaz tipo) {
    printf("Configuración:\n");
    printf("Tipo de interfaz: %s\n", config->tipo_interfaz);
    
    // Condicional para imprimir tiempo_unidad_trabajo si el tipo de interfaz no es STDIN
    if (tipo != STDIN) {
        printf("Tiempo por unidad de trabajo: %d\n", config->tiempo_unidad_trabajo);
    }
    
    printf("IP del kernel: %s\n", config->ip_kernel);
    printf("Puerto del kernel: %s\n", config->puerto_kernel);
    
    // Condicional para imprimir IP y puerto de memoria si el tipo de interfaz no es GENERICA
    if (tipo != GENERICA) {
        printf("IP de memoria: %s\n", config->ip_memoria);
        printf("Puerto de memoria: %s\n", config->puerto_memoria);
        
        // Condicional adicional para imprimir elementos específicos de DIALFS
        if (tipo == DIALFS) {
            printf("Ruta base de DialFS: %s\n", config->path_base_dialfs);
            printf("Tamaño del bloque: %d\n", config->block_size);
            printf("Cantidad de bloques: %d\n", config->block_count);
            printf("Tiempo de retraso por cada compactacion: %d\n\n", config->retraso_compactacion);
        }
    }
}
