#ifndef LOGGING_H_
#define LOGGING_H_

#include <stdlib.h>
#include <commons/log.h>
#include <stdbool.h>

// crea un log activo por consola
t_log* iniciar_logger(char* file, char* nombre_proceso, t_log_level nivel);

// crea un log el cual no se muestra por consola
t_log* iniciar_logger_oculto(char* file, char* nombre_proceso, t_log_level nivel);

#endif