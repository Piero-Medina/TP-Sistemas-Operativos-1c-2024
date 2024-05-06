#ifndef GENERALES_H_
#define GENERALES_H_

#include <commons/log.h>
#include "kernel_config.h"
#include <stdbool.h>
#include <semaphore.h>
#include <stdint.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>

#include <enum/enum.h>


typedef enum{
    FIFO,
    RR,
    VRR
}algoritmo;

typedef struct {
	int instancias;
	t_queue* cola_recurso;
}t_recurso;

typedef struct {
	int tipo;
	int socket;
	bool ocupado;
}t_interfaz;

typedef struct {
	int pid;
	int operacion;
	t_interfaz* interfaz;
	char* parametro_string;
	int parametro_int_1;
	int parametro_int_2;
	int parametro_int_3;
	int parametro_int_4;
}t_io_pendiente;

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
extern pthread_t hilo_cpu_dispatch;

/**
 * @DESC: variables declaradas en init.c
*/

//
extern int grado_multiprogramacion_global;
extern bool procesar_conexion_en_ejecucion;
extern int contador_pid;
extern algoritmo algoritmo_elegido;

extern bool proceso_en_ejecucion; 

// semaforos
extern sem_t mutex_conexion_memoria;
extern sem_t mutex_conexion_cpu_dispatch;
extern sem_t mutex_conexion_cpu_interrupt;

extern sem_t sem_grado_multiprogramacion;
extern sem_t mutex_pid;
extern sem_t sem_procesos_esperando_en_new;
extern sem_t sem_procesos_esperando_en_ready;

extern sem_t sem_cpu_disponible;

extern sem_t mutex_cola_new;
extern sem_t mutex_cola_ready;
extern sem_t mutex_cola_ready_aux;
extern sem_t mutex_cola_execute;
extern sem_t mutex_cola_blocked;
extern sem_t mutex_cola_exit;

extern sem_t mutex_proceso_en_ejecucion;

extern sem_t mutex_diccionario_interfaces;

// colas
extern t_queue* cola_new;
extern t_queue* cola_ready;
extern t_queue* cola_ready_aux;
extern t_queue* cola_execute;
extern t_queue* cola_blocked;
extern t_queue* cola_exit;

// diccionario
extern t_dictionary* recursos;
extern t_dictionary* interfaces;

// hilo
extern pthread_t hilo_planificador_LP;
extern pthread_t hilo_planificador_CP;

#endif 