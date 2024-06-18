#ifndef PROCESAR_CONEXION_H_
#define PROCESAR_CONEXION_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h> 

#include "generales.h"
#include "archivos_dialFs.h"

#include <enum/enum.h>
#include <conexiones/conexiones.h>
#include <comunicacion/comunicacion.h>
#include <utils/utils.h>

#include <commons/collections/list.h>


void procesar_conexion_siendo_io_generica(void *args);
void procesar_conexion_siendo_io_dialFs(void *args);

#endif 