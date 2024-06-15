#ifndef CICLO_DE_INSTRUCCION
#define CICLO_DE_INSTRUCCION

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <sys/time.h>
#include <stdint.h>

#include "generales.h"
#include "mmu.h"

#include <pcb/pcb.h>
#include <instruccion/instruccion.h>
#include <comunicacion/comunicacion.h>
#include <enum/enum.h>
#include <utils/utils.h>

// ejecutamos un clico de intruccion hasta que surga un evento
void ejecutar_ciclo_de_instruccion(int conexion, t_PCB* pcb);

// establece el tiempo restante (en milisegundos) de ejecucion que le queda a un proceso justo despues de terminar su ejecucion
void establecer_tiempo_restante_de_ejecucion(t_PCB* pcb,struct timeval inicio, struct timeval final);

void incrementar_program_counter(t_PCB* pcb, int en_cuanto);

#endif 