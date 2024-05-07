#ifndef PETICIONES_KERNEL_H_
#define PETICIONES_KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "generales.h"

#include <commons/collections/list.h>
#include <commons/string.h>

#include <instruccion/instruccion.h>
#include <utils/utils.h>
#include <comunicacion/comunicacion.h>

#define MAX_READ 50

// crea un proceso y la agrega a la lista_de_procesos
void kernel_creacion_nuevo_proceso(int conexion);

// crea un proceso segun un PID y una ruta que contiene un (archivo.txt)
t_proceso* crear_proceso_mediante_path(int pid, char* path_intrucciones);

// verifica si es una instruccion permitida (definidas en t_parser)
bool instruccion_permitida(char** split, int posicion);

// verifica si existe el token y devuelve la posicion en la que se encontro el token (definidas en t_parser)
bool existe_Token(char* token, int* pos);

// verifica si la cantidad de parametros para una pisicion coincide (definidas en t_parser)
bool cantidad_parametros_correctos(char** split, int posicion);

void imprimir_array_strings(char** array);

// Función para imprimir cada proceso en una lista de procesos (t_list* lista que contiene t_proceso*)
void imprimir_lista_procesos(t_list* lista_procesos);
// Función para imprimir todos los elementos de un proceso (t_proceso*)
void imprimir_proceso(void* proceso_ptr);

t_proceso* buscar_proceso_por_pid_y_remover(int pid, t_list* lista);
int posicion_de_proceso_por_pid(int pid, t_list* lista);

#endif 