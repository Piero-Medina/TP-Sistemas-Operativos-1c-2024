#ifndef PETICIONES_CPU_H
#define PETICIONES_CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <stdint.h>

#include "generales.h"
#include "pagina.h"
#include "peticiones_kernel.h"

#include <instruccion/instruccion.h> 
#include <utils/utils.h>

// busca una intruccion segun el pid y el program counter del proceso y la devuelve
t_instruccion* buscar_intruccion(int pid, int program_counter);

// busca un proceso por medio de su pid y la devuelve
t_proceso* buscar_proceso(int pid);

int32_t buscar_marco(int pid, int numero_pagina);

int resize_proceso(uint32_t pid, uint32_t tamanio);
void crear_tabla_de_paginas(uint32_t pid ,uint32_t tamanio);
void ampliar_proceso(uint32_t  pid, uint32_t tamanio);
void reducir_proceso(uint32_t  pid, uint32_t tamanio);
void vaciar_proceso(uint32_t pid);

#endif 