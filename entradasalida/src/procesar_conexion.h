#ifndef PROCESAR_CONEXION_H_
#define PROCESAR_CONEXION_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "generales.h"

#include <enum/enum.h>
#include <conexiones/conexiones.h>
#include <comunicacion/comunicacion.h>
#include <utils/utils.h>


void procesar_conexion_siendo_io_generica(void *args);

#endif 