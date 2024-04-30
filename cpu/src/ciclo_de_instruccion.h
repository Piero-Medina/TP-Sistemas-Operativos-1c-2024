#ifndef CICLO_DE_INSTRUCCION
#define CICLO_DE_INSTRUCCION

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <sys/time.h>

#include "generales.h"

#include <pcb/pcb.h>
#include <instruccion/instruccion.h>
#include <comunicacion/comunicacion.h>
#include <enum/enum.h>

// ejecutamos un clico de intruccion hasta que surga un evento
void ejecutar_ciclo_de_instruccion(int conexion, t_PCB* pcb);

#endif 