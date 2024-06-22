#ifndef STD_H
#define STD_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h> 
#include <readline/readline.h>
#include <readline/history.h>

#include <commons/collections/list.h>
#include <commons/log.h>

#include <peticion_memoria/peticion_memoria.h>
#include <utils/utils.h>

#include "generales.h"

void leer_de_teclado(uint32_t pid, uint32_t bytes, t_list* direcciones_fisicas);

void imprimir_por_pantalla(uint32_t pid, uint32_t bytes, t_list* direcciones_fisicas);

#endif 