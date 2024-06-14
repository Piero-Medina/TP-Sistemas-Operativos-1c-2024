#ifndef INIT_H_
#define INIT_H_

#include <signal.h>
#include <semaphore.h>
#include <pthread.h>

#include "generales.h"
#include "procesar_conexion.h"
#include "tlb.h"

#include <conexiones/conexiones.h>

void init_cpu(void);

void escuchar_dispatch(void *arg);
void escuchar_interrupt(void *arg);

void init_semaforos(void);
void liberar_semaforos(void);

// liberar de manera correcta ante (Ctrl+C)
void sigint_handler(int signum);

// liberamos - server - conexiones principales - log - config
void liberar_cpu(void);

void init_tlb(void);

void algorimo_usado(void);

#endif 