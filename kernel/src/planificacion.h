#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include <stdio.h>
#include <stdlib.h>
#include "generales.h"
#include <commons/log.h>
#include <semaphore.h>
#include <commons/collections/queue.h>

#include <pcb/pcb.h>
#include <comunicacion/comunicacion.h>
#include <enum/enum.h>

// movemos un proceso recien creado a la cola new
void mover_a_new(t_PCB* pcb);

// new - exit
void func_largo_plazo(void* arg);
// ready - exec - blocked
void func_corto_plazo(void* arg);

// movemos un proceso de new a ready
void mover_new_a_ready(void);

// movemos un proceso de ready a execute
void mover_ready_a_execute(void);

// movemos un proceso de execute a blocked
void mover_execute_a_blocked(t_PCB* pcb);

// actualiza el contexto de ejecucion (guiño guiño)
t_PCB* actualizar_contexto(t_PCB* pcb_nueva, t_PCB* pcb_vieja);

// movemos un proceso de blocked a ready
void mover_blocked_a_ready(void);

#endif 