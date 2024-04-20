#ifndef CONSOLA_INTERACTIVA_H_
#define CONSOLA_INTERACTIVA_H_

#include <stdio.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <commons/string.h>
#include <string.h>
#include <semaphore.h>

#include "generales.h"
#include "planificacion.h"

#include <comunicacion/comunicacion.h>
#include <pcb/pcb.h>
#include <utils/utils.h>

#define MAX 30

typedef enum{
    EJECUTAR_SCRIPT,
    INICIAR_PROCESO,
    FINALIZAR_PROCESO,
    DETENER_PLANIFICACION,
    INICIAR_PLANIFICACION,
    PROCESO_ESTADO
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

// abre un archivo en modo lectura
FILE* leer_archivo(char *path);
void cerrar_archivo(FILE* archivo);

// func_comandos
void func_ejecutar_script(char* leido);
void func_iniciar_proceso(char* leido);
void func_finalizar_proceso(char* leido);
void func_detener_planificacion(void);
void func_iniciar_planificacion(void);
void func_proceso_estado(void);

#endif 