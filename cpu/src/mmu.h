#ifndef MMU_H
#define MMU_H
  
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

#include "generales.h"
#include "tlb.h"

#include <comunicacion/comunicacion.h>
#include <enum/enum.h>
#include <peticion_memoria/peticion_memoria.h>

// - Traduce las peticiones requeridas para que a partir de una direccion Logica se Escriban/Lean ciertos bytes.
// - Retorna el estado en el que quedo la traduccion para actuar en cada caso.
// - traduccion correcta (MMU_OK) | no existe el nro de pagina (SEGMENTATION_FAULT) se cancela traduccion
int MMU(int direccion_logica, int tamanio_pagina, uint32_t pid, uint32_t bytes, t_list* peticiones);

// - Traduce una direccion fisica a paritr de un numero de pagina y desplazamiento.
int obtener_direccion_fisica(uint32_t numero_pagina, uint32_t pid, int desplazamiento, int tamanio_pagina, uint32_t* direccion_fisica);


#endif 