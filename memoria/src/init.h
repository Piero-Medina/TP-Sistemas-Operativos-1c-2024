#ifndef INIT_H_
#define INIT_H_

#include "generales.h"
#include <commons/log.h>
#include "memoria_config.h"
#include <conexiones/conexiones.h>
#include <signal.h>
#include <commons/collections/list.h>

#include <instruccion/instruccion.h>

void init_memoria(void);

void sigint_handler(int signum);

void liberar_memoria(void);

void init_estructura_para_parsear(void);

void init_lista_de_procesos(void);
void liberar_lista_de_procesos(void);
void liberar_elemento_proceso(void* elemento);

#endif 