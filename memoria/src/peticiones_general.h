#ifndef PETICIONES_GENERAL_H
#define PETICIONES_GENERAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "generales.h"
 
#include <utils/utils.h>

void* get_valor_memoria(uint32_t direcion_fisica, uint32_t tamanio);
void set_valor_en_memoria(uint32_t direcion_fisica, void* data, uint32_t tamanio);

#endif 