#ifndef PROCESAR_CONEXION_H_
#define PROCESAR_CONEXION_H_

#include <conexiones/conexiones.h>
#include <enum/enum.h>
#include <serializacion/serializacion.h>
#include "generales.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <commons/collections/dictionary.h>

#include <comunicacion/comunicacion.h>
#include <pcb/pcb.h>
#include <utils/utils.h>
#include "planificacion.h"
#include "init.h"
#include "manejo_interfaz.h"


// procesamos las conexiones que nos lleguen de una interfaz io
void procesar_conexion_io(void *args);

// procesamos la conexion que nos llega del modulo CPU_DISPATCH
void procesar_conexion_cpu_dispatch(void *args);

// funciones para simular mandar a io

// crea un hilo detach
void hilo_procesar_io_fake(int milisegundos);
// duerme al hilo por un tiempo y luego mueve el proceso de blocked a ready
void procesar_io_fake(void* arg);

#endif 