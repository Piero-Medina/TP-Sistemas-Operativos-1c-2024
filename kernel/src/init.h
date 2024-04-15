#ifndef INIT_H_
#define INIT_H_

#include "generales.h"
#include <commons/log.h>
#include "kernel_config.h"
#include <signal.h>
#include "procesar_conexion.h"
#include <semaphore.h>
#include <stdint.h>
#include <commons/collections/queue.h>

#include <conexiones/conexiones.h>
#include <pcb/pcb.h>

// 
void init_kernel(void);

// liberar de manera correcta ante (Ctrl+C)
void sigint_handler(int signum);

// liberamos - server - conexiones principales - log - config
void liberar_kernel(void);

// 
void escuchar_kernel(void *arg);

// 
void init_semaforos(void);
void liberar_semaforos(void);

//
void init_colas(void);
void liberar_colas(void);

void liberar_elemento_pcb(void* data);

#endif 