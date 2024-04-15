#ifndef GENERALES_H_
#define GENERALES_H_

#include <commons/log.h>
#include "kernel_config.h"
#include <stdbool.h>
#include <semaphore.h>
#include <stdint.h>
#include <commons/collections/queue.h>


/**
 * @DESC: variables declaradas en main.c
*/

// 
extern t_log* logger;
extern t_kernel_config* config;

// conexiones
extern int conexion_cpu_dispatch;
extern int conexion_cpu_interrupt;
extern int conexion_memoria;
extern int server_fd;

// hilo
extern pthread_t hilo_server_kernel;

/**
 * @DESC: variables declaradas en init.c
*/

//
extern bool procesar_conexion_en_ejecucion;
extern int contador_pid;

// semaforos
extern sem_t mutex_conexion_memoria;
extern sem_t sem_grado_multiprogramacion;
extern sem_t mutex_pid;
extern sem_t mutex_cola_new;

// colas
extern t_queue* cola_new;

#endif 