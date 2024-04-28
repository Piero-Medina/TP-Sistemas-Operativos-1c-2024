#ifndef INSTRUCCION_H_
#define INSTRUCCION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commons/collections/list.h>
#include <commons/string.h>

typedef enum {
    SET,                // 2 parámetros
    SUM,                // 2 parámetros
    SUB,                // 2 parámetros
    MOV_IN,             // 2 parámetros
    MOV_OUT,            // 2 parámetros
    RESIZE,             // 1 parámetro
    JNZ,                // 2 parámetros
    COPY_STRING,        // 1 parámetros
    IO_GEN_SLEEP,       // 2 parámetros
    IO_STDIN_READ,      // 3 parámetros
    IO_STDOUT_WRITE,    // 3 parámetros
    IO_FS_CREATE,       // 2 parámetros
    IO_FS_DELETE,       // 2 parámetros
    IO_FS_TRUNCATE,     // 3 parámetros
    IO_FS_WRITE,        // 5 parámetros
    IO_FS_READ,         // 5 parámetros
    WAIT,               // 1 parámetro
    SIGNAL,             // 1 parámetro
    EXIT_I              // 0 parámetros
}t_identificador;

typedef struct {
    t_identificador identificador;
    t_list* parametros; // lista de string (cada string es un parametro)
}t_instruccion;

// se define las instrucciones permitidas
typedef struct{
	char* intruccion;
	int cant_parametros;
}t_parser;

t_identificador convertir_a_identificador(char* identificador);

t_instruccion* crear_instruccion_mediante_array_string(char** split);

// Función para imprimir una instrucción (t_intruccion*)
void imprimir_instruccion(void* instruccion_ptr);
// Función para imprimir los parámetros de una intruccion (char*)
void imprimir_parametros(void* parametro);

// liberar una lista de intrucciones
void destruir_list_instrucciones(t_list* instrucciones);
// liberar una intruccion
void destruir_elemento_instruccion(void* elemento);
 
#endif