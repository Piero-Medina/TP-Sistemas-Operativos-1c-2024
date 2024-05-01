#ifndef PETICIONES_CPU_H
#define PETICIONES_CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/collections/list.h>

#include "generales.h"

#include <instruccion/instruccion.h> 

// busca una intruccion segun el pid y el program counter del proceso y la devuelve
t_instruccion* buscar_intruccion(int pid, int program_counter);

// busca un proceso por medio de su pid y la devuelve
t_proceso* buscar_proceso(int pid);
#endif 