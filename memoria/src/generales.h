#ifndef GENERALES_H_
#define GENERALES_H_

#include <commons/log.h>
#include "memoria_config.h"
#include <stdbool.h>

// declarado en main.c
extern t_log* logger;
extern t_memoria_config* config;

extern int server_fd;

// declarado en init.c
extern bool procesar_conexion_en_ejecucion;

#endif 