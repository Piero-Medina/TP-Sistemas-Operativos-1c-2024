#ifndef MMU_H_
#define MMU_H_
  
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>

#include <commons/log.h>
#include <commons/config.h>

#include "generales.h"
#include "tlb.h"

#include <comunicacion/comunicacion.h>
#include <enum/enum.h>

// - Traduce un numero de pagina a una Direccion fisica
// - Retorna el estado en el que quedo la traduccion para actuar en cada caso.
// - traduccion correcta (MMU_OK) | no existe el nro de pagina (SEGMENTATION_FAULT) se cancela traduccion
int mmu(int direccion_logica, int tamanio_pagina, uint32_t pid, uint32_t* direccion_fisica);


#endif 