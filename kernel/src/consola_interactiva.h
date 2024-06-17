#ifndef CONSOLA_INTERACTIVA_H_
#define CONSOLA_INTERACTIVA_H_

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <commons/string.h>
#include <string.h>
#include <semaphore.h>

#include "generales.h"
#include "planificacion.h"

#include <comunicacion/comunicacion.h>
#include <pcb/pcb.h>
#include <utils/utils.h>
#include <io_pendiente/io_pendiente.h>

#define MAX 200

typedef enum{
    EJECUTAR_SCRIPT,
    INICIAR_PROCESO,
    FINALIZAR_PROCESO,
    DETENER_PLANIFICACION,
    INICIAR_PLANIFICACION,
    MODIFICAR_MULTIPROGRAMACION,
    PROCESO_ESTADO,
    OPCION_NEGADA
} opcion_consola;

typedef struct{
    opcion_consola opcion;
    int cant_parametros;
} t_opcion_consola;

void consola_interactiva(void);
void menu(void);
void procesar_opcion(char* leido);
t_opcion_consola opcion_seleccionada(char* leido);
void ejecutar_opcion(t_opcion_consola opcion, char* leido);


// func_comandos
void func_ejecutar_script(char* leido);
void func_iniciar_proceso(char* leido);
void func_finalizar_proceso(char* leido);
void func_detener_planificacion(void);
void func_iniciar_planificacion(void);
void func_modificar_multiprogramacion(char* leido);
void func_proceso_estado(void);

void imprimir_recursos_bloqueados(void);

bool finalizar_proceso_NEW(int pid);
bool finalizar_proceso_READY(int pid);
bool finalizar_proceso_READY_AUX(int pid);
bool finalizar_proceso_EXECUTE(int pid);
bool finalizar_proceso_BLOCKED(int pid);
bool finalizar_proceso_BLOCKED_RECURSO(int pid);
void finalizar_proceso_EXIT(int pid);

#endif 