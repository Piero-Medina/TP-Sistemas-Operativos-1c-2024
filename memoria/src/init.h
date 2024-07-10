#ifndef INIT_H_
#define INIT_H_

#include <commons/log.h>
#include <conexiones/conexiones.h>
#include <signal.h>
#include <commons/collections/list.h>
#include <semaphore.h>

#include "generales.h"
#include "memoria_config.h"
#include "bit_map.h"
#include "pagina.h"

#include <instruccion/instruccion.h>

void init_memoria(void);

void sigint_handler(int signum);

void liberar_memoria(void);

void init_semaforos(void);
void liberar_semaforos(void);

void init_estructura_para_parsear(void);

void init_lista_de_procesos(void);
void liberar_lista_de_procesos(void);
void liberar_elemento_proceso(void* elemento);

void init_memoria_real(void);
void liberar_memoria_real(void);

void init_bitmap(void);
void liberar_bitmap(void);

#endif 