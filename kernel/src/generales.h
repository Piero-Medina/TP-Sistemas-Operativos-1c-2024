#ifndef GENERALES_H_
#define GENERALES_H_

#include <commons/log.h>
#include "kernel_config.h"
#include <stdbool.h>

// declarado en main.c
extern t_log* logger;
extern t_kernel_config* config;

extern int conexion_cpu_dispatch;
extern int conexion_cpu_interrupt;
extern int conexion_memoria;
extern int server_fd;

extern pthread_t hilo_server_kernel;

// declarado en init.c
extern bool procesar_conexion_en_ejecucion;


#endif 