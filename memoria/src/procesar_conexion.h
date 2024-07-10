#ifndef PROCESAR_CONEXION_H_
#define PROCESAR_CONEXION_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h> 

#include <conexiones/conexiones.h>
#include <enum/enum.h>
#include <serializacion/serializacion.h>
#include <comunicacion/comunicacion.h>
#include <utils/utils.h>

#include "generales.h"
#include "peticiones_kernel.h"
#include "peticiones_cpu.h"
#include "peticiones_general.h"
#include "init.h"

void procesar_conexion_general(void *args);

#endif 