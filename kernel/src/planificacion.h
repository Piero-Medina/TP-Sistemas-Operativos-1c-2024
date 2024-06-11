#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include <stdio.h>
#include <stdlib.h>
#include "generales.h"
#include <commons/log.h>
#include <semaphore.h>
#include <commons/collections/queue.h>
#include <stdbool.h>

#include <pcb/pcb.h>
#include <comunicacion/comunicacion.h>
#include <enum/enum.h>
#include <utils/utils.h>

#include <manejo_interfaz.h>

// movemos un proceso recien creado a la cola new
void mover_a_new(t_PCB* pcb);

// new - exit
void func_largo_plazo(void* arg);
// ready - exec - blocked
void func_corto_plazo(void* arg);

// movemos un proceso de new a ready
void mover_new_a_ready(void);

// movemos un proceso de ready a execute
void mover_ready_a_execute(uint32_t* pid);

// movemos un proceso de execute a blocked (dentro se actualiza el contexto)
void mover_execute_a_blocked(t_PCB* pcb);

// actualiza el contexto de ejecucion (guiño guiño)
t_PCB* actualizar_contexto(t_PCB* pcb_nueva, t_PCB* pcb_vieja);

// movemos un proceso de blocked a ready | luego ver si hace falta actualizar el contexto
void mover_blocked_a_ready(int pid);

// movemos un proceso de execute a ready (dentro se actualiza el contexto)
void mover_execute_a_ready(t_PCB* pcb_nueva);

// - mover un proceso de la cola ready de mayor prioridad a execute (VRR)
void mover_ready_aux_a_execute(uint32_t* pid, uint32_t* quantum_restante);

// - mover un proceso de execute a la cola ready de mayor prioridad (VRR)
void mover_execute_a_ready_aux(t_PCB* pcb_nueva);

// - mover un proceso de Blocked a la cola ready de mayor prioridad (VRR)
void mover_blocked_a_ready_aux(int pid);

/*
    dado que podemos mandar a exit desde diferentes estados 
    (new, ready, execute, blocked). esta funcion se encarga de
    loggear el estado anterior en el que estaba el proceso y de hacer
    todo lo requerido SOLO PARA MANDAR A EXIT A UN PROCESO.

    sacarlo de la cola anterior en la que estaba es tarea que se
    realizara manualmente, almenos que se quiera hacer una funcion
    para cada transicion.  
*/
void mandar_a_exit(t_PCB* pcb, char* motivo);

// movimientos a exit
void mover_execute_a_exit(t_PCB* pcb, char* motivo);

// - Devuelve un proceso a execute
// - Se encarga de cumplir el algoritmo actual que maneja el Kernel (mediante un hilo)
// - Actualiza el contexto de ejecucion
void devolver_a_execute(t_PCB* pcb);
void mini_planificador_corto_plazo(void* arg);
void mover_execute_a_execute(uint32_t* pid, uint32_t* quantum_restante);
// - Mueve a la cola ready cumpliendo el agoritmo actual (el pcb debe estar actualizado de antemano)
void mover_a_ready_o_ready_aux(t_PCB* pcb);

#endif 