#ifndef PROCESAR_CONEXION_H_
#define PROCESAR_CONEXION_H_

#include <conexiones/conexiones.h>
#include <enum/enum.h>
#include <serializacion/serializacion.h>
#include "generales.h"
#include <semaphore.h>
#include <stdbool.h>
#include <commons/collections/dictionary.h>

#include <comunicacion/comunicacion.h>
#include <pcb/pcb.h>
#include <utils/utils.h>
#include "planificacion.h"
#include "init.h"


// procesamos las conexiones que nos lleguen de una interfaz io
void procesar_conexion_io(void *args);

// procesamos la conexion que nos llega del modulo CPU_DISPATCH
void procesar_conexion_cpu_dispatch(void *args);

// agregamos una t_interfaz al diccionario 
void agregar_interfaz(t_dictionary* interfaces, char* nombre_interfaz, int conexion, int tipo_interfaz);

#endif 