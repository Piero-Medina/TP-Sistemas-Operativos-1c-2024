#ifndef PROCESAR_CONEXION_H_
#define PROCESAR_CONEXION_H_

#include <stdbool.h>
#include <semaphore.h>

#include <conexiones/conexiones.h>
#include <enum/enum.h>
#include <serializacion/serializacion.h>
#include <pcb/pcb.h>
#include <comunicacion/comunicacion.h>

#include "generales.h"
#include "ciclo_de_instruccion.h"


void procesar_conexion_kernel(void *args);

#endif 