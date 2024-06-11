#ifndef BLOQUES_DIALFS_H_
#define BLOQUES_DIALFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> // open() ...
#include <string.h>

#include "generales.h"

#include <utils/utils.h>

#define MAX_READ 4096

// crea un archivo de bloques
void crear_archivo_de_bloques(char* path_bloques);

FILE* abrir_archivo_de_bloques(void);
FILE* abrir_archivo_existente(char* path);
FILE* crear_archivo_nuevo(char* path);

void escribir_n_cantidad_de_caracter(FILE* file, char caracter, int cantidad);

// -El string debe tener el caracter nulo ('\0')
void escribir_string(FILE* file, char* string);

void escribir_string_n_bytes(FILE* file, char* string, int bytes);

// - lee n cantidad de caracteres y agrega al final el caracter nulo
// - (liberar memoria)
char* leer_n_cantidad_de_caracter(FILE* file, int cantidad_bytes);

// -se empieza a mover desde el comienzo del bloque
void mover_puntero_de_archivo_por_bloques(FILE* file, int posicion);

void cerrar_archivo_de_bloques(FILE* file);

// - retorna la ruta de la copia del archivo de bloques
char* crear_copia_archivo_de_bloque(void);
void eliminar_copia_archivo_de_bloque(void);

#endif