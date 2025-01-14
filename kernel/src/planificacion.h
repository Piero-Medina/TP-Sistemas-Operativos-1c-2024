#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
bool mover_execute_a_blocked(t_PCB* pcb);

// actualiza el contexto de ejecucion (guiño guiño)
t_PCB* actualizar_contexto(t_PCB* pcb_nueva, t_PCB* pcb_vieja);

// movemos un proceso de blocked a ready | luego ver si hace falta actualizar el contexto
void mover_blocked_a_ready(int pid);

// movemos un proceso de execute a ready (dentro se actualiza el contexto)
bool mover_execute_a_ready(t_PCB* pcb_nueva);

// - mover un proceso de la cola ready de mayor prioridad a execute (VRR)
void mover_ready_aux_a_execute(uint32_t* pid, uint32_t* quantum_restante);

// - mover un proceso de execute a la cola ready de mayor prioridad (VRR)
bool mover_execute_a_ready_aux(t_PCB* pcb_nueva);

// - mover un proceso de Blocked a la cola ready de mayor prioridad (VRR)
void mover_blocked_a_ready_aux(int pid);

/*
    dado que podemos mandar a exit desde diferentes estados 
    (new, ready, execute, blocked). esta funcion se encarga de
    loggear el estado anterior en el que estaba el proceso y de hacer
    todo lo requerido SOLO PARA MANDAR A EXIT A UN PROCESO.

    - logear si esque hay un motivo (Finaliza el proceso PID: <%u> - Motivo: <%s>)
    - logea y solicita (Solicitando a MEMORIA que libere estructuras asocidas al proceso PID: <%u>)
    - logea y libera todos los recursos retenidos por el proceso PID, solo si hay recursos disponibles en
      el sistema (Liberando recursos retenidos por el proceso PID: <%u>)
    - logea el estado anterior antes de ser mandado a exit (PID: <%u> - Estado Anterior: <%s> - Estado Actual: <EXIT>)
    - finalmente ingresa el proceso a la cola exit

    sacarlo de la cola anterior en la que estaba es tarea que se
    realizara manualmente, ademas de actualizar el contexto, 
    almenos que se quiera hacer una funcion para cada transicion.  
*/
void mandar_a_exit(t_PCB* pcb, char* motivo);

// - movimientos a exit
bool mover_execute_a_exit(t_PCB* pcb, char* motivo);

// - moviemientos a exit
// - Como antes de meter a blocked el proceso el pcb es actualizado (execute a blocked), no hace 
//   falta actualizar el pcb, por lo que solo con el pid puedo sacar el pcb de blocked. 
void mover_blocked_a_exit(int pid, char* motivo);

// - Devuelve un proceso a execute
// - Se encarga de cumplir el algoritmo actual que maneja el Kernel (mediante un hilo)
// - Actualiza el contexto de ejecucion
void devolver_a_execute(t_PCB* pcb);
void mini_planificador_corto_plazo(void* arg);
void mover_execute_a_execute(uint32_t* pid, uint32_t* quantum_restante);
// - Mueve a la cola ready cumpliendo el agoritmo actual (el pcb debe estar actualizado de antemano)
void mover_a_ready_o_ready_aux(t_PCB* pcb);

void agregar_registro_recurso(uint32_t pid, char* recurso);
void eliminar_registro_recurso(uint32_t pid, char* recurso);
void devolver_recursos(uint32_t pid);
void eliminar_devueltos(void);


/// @brief Obtiene una cadena que describe un motivo de salida según el enum motivo_exit.
///        Esta función toma un valor del tipo motivo_exit y opcionalmente un nombre adicional,
///        y devuelve un string asignado dinámicamente que describe el motivo de salida.
///
/// @param motivo El motivo de salida del tipo motivo_exit.
/// @param nombre Opcional. Nombre adicional para agregar al motivo de salida.
/// @return Un puntero a un string asignado dinámicamente que describe el motivo de salida.
///         Es responsabilidad del llamador liberar esta memoria utilizando free().
/// @note Ejemplos:
/// @note - Para motivo SALIDA_SUCCESS y nombre "Archivo guardado": devuelve "SUCCESS (Archivo guardado)".
/// @note - Para motivo SALIDA_OUT_OF_MEMORY sin nombre: devuelve "OUT_OF_MEMORY".
char* obtener_motivo_salida(motivo_exit motivo, const char* nombre);



#endif 