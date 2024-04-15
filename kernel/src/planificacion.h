#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include <stdio.h>
#include <stdlib.h>
#include "generales.h"
#include <commons/log.h>
#include <semaphore.h>
#include <commons/collections/queue.h>

#include <pcb/pcb.h>

// movemos un proceso recien creado a la cola new
void mover_a_new(t_PCB* pcb);

#endif 