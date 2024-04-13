#ifndef INIT_H_
#define INIT_H_

#include "generales.h"
#include <pthread.h>
#include <conexiones/conexiones.h>
#include "procesar_conexion.h"
#include <signal.h>
#include <semaphore.h>

void init_cpu(void);

void escuchar_dispatch(void *arg);
void escuchar_interrupt(void *arg);

void init_semaforos(void);
void liberar_semaforos(void);

// liberar de manera correcta ante (Ctrl+C)
void sigint_handler(int signum);

// liberamos - server - conexiones principales - log - config
void liberar_cpu(void);

#endif 