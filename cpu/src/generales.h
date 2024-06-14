#ifndef GENERALES_H_
#define GENERALES_H_

#include "cpu_config.h"
#include "tlb.h"
#include <commons/log.h>
#include <stdbool.h>
#include <semaphore.h>

// declarado en main.c
extern t_cpu_config* config;
extern t_log* logger;

extern int conexion_memoria;
extern int server_cpu_dispatch_fd; 
extern int server_cpu_interrupt_fd;

// declarado en init.c

extern bool procesar_conexion_en_ejecucion;
extern bool desalojo;
extern bool tlb_habilitada;

extern algoritmo algoritmo_elegido;

extern sem_t mutex_desalojo;

extern t_list* tlb;

#endif 