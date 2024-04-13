#ifndef INIT_H_
#define INIT_H_

#include "generales.h"
#include <commons/log.h>
#include "memoria_config.h"
#include <conexiones/conexiones.h>
#include <signal.h>

void init_memoria(void);

void sigint_handler(int signum);

void liberar_memoria(void);

#endif 