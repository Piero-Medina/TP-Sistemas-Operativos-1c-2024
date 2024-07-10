#ifndef GENERALES_H_
#define GENERALES_H_

#include <stdlib.h>
#include <commons/log.h>
#include <stdbool.h>
#include <semaphore.h>

#include <instruccion/instruccion.h>
#include <commons/collections/list.h>

#include "memoria_config.h"
#include "bit_map.h"

typedef struct {
    int pid;
    t_list* instrucciones; // (t_instruccion*)
    t_list* tabla_paginas; // (t_paginas*)
    int tamanio; // (bytes)
}t_proceso;

// declarado en main.c
extern t_log* logger;
extern t_memoria_config* config;

extern int server_fd;

// declarado en init.c

extern sem_t mutex_lista_de_procesos;
extern sem_t mutex_memoria_real;

extern bool procesar_conexion_en_ejecucion;

extern t_parser tabla[19];

extern t_list* lista_de_procesos; // (t_proceso*)

extern void* memoria_real;

extern t_bitmap* bitmap;

#endif 