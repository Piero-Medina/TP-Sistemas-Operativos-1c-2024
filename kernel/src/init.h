#ifndef INIT_H_
#define INIT_H_

#include "generales.h"
#include <commons/log.h>
#include "kernel_config.h"
#include <conexiones/conexiones.h>
#include <signal.h>
#include "procesar_conexion.h"


void init_kernel(void);

// liberar de manera correcta ante (Ctrl+C)
void sigint_handler(int signum);

// liberamos - server - conexiones principales - log - config
void liberar_kernel(void);

//
void escuchar_kernel(void *arg);

#endif 