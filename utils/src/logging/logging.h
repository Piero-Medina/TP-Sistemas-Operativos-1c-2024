#ifndef LOGGING_H_
#define LOGGING_H_

#include <stdlib.h>
#include <commons/log.h>
#include <stdbool.h>

// crea un log activo por consola
t_log* iniciar_logger(char* path, char* nombre_proceso);

// crea un log el cual no se muestra por consola
t_log* iniciar_logger_oculto(char* file, char* nombre_proceso);

#endif