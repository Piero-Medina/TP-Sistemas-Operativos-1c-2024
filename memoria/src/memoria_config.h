#ifndef MEMORIA_CONFIG_H
#define MEMORIA_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>

// Definición de la estructura para la configuración de memoria
typedef struct {
    t_config* config;
    char* ip_memoria;
    char* puerto_escucha_memoria;
    int tam_memoria;
    int tam_pagina;
    char* path_instrucciones;
    int retardo_respuesta;
} t_memoria_config;

// Función para inicializar la configuración de memoria desde un archivo
t_memoria_config* init_memoria_config(char* path);

// Función para liberar la memoria asignada a la configuración de memoria
void liberar_memoria_config(t_memoria_config* memoria_config);

#endif 
