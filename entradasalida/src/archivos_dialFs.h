#ifndef ARCHIVOS_DIALFS_H_
#define ARCHIVOS_DIALFS_H_

/**
 * @brief si existe el Metadato tambien debe existir el Archivo Fisico
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> // open() ...
#include <string.h>
#include <dirent.h> // recorrer directorio

#include <commons/collections/list.h>

#include "generales.h"
#include "bloques_dialFs.h"
#include "bit_map_dialFs.h"

#include <utils/utils.h>
#include <comunicacion/comunicacion.h>
#include <peticion_memoria/peticion_memoria.h>

#define MAX_READ 4096

estado_interfaz crear_archivo(char* nombre);
estado_interfaz truncar_archivo(char* nombre, int tamanio_nuevo, int pid);
estado_interfaz eliminar_archivo(char* nombre);
estado_interfaz escribir_archivo(char* nombre_archivo, uint32_t bytes, uint32_t puntero, t_list* direcciones, uint32_t pid);
estado_interfaz leer_archivo_dial(char* nombre_archivo, uint32_t bytes, uint32_t puntero, t_list* direcciones, uint32_t pid);

t_archivo* t_archivo_crear(char* nombre, int bloque_inicial);
void imprimir_t_archivo(t_archivo* archivo);
void liberar_elemento_t_archivo(void* archivo);

// crea el metadato de una t_archivo
void crear_metadata(t_archivo* archivo);

// escribe el bloque inicial en el archivo de bloques
// ademas crea el archivo fisico y tambien escribe un bloque (relativo)
void escribir_primer_bloque(FILE* archivo_de_bloques, t_archivo* archivo);

// se levanta todo un archivo a partir del directorio de metadatos.
// - se asume que su archivo Fisico ya fue creado (o sino habria que recorrer dos veces para verificar)
void levantar_diccionario_de_archivos(t_dictionary* archivos);

// levanta un archivo asumiendo que existe su archivo fisico
// - se asumen que recibe el nombre del archivo metadato (sin la ruta)
// - se asumen que la extension del archivo fisico es .txt
t_archivo* levantar_t_archivo(char* nombre_sin_extension);

// truncado
void reducir_archivo(t_archivo* archivo, int tamanio_bytes, int tamanio_bloque);
void vaciar_archivo(t_archivo* archivo);
void ampliar_archivo(t_archivo* archivo, int nuevo_tamanio_bytes, int tamanio_bloque, int pid);
void sub_ampliar_archivo(t_archivo * archivo, int nuevo_tamanio_bytes);
void diferenciar_de_magia_archivo(t_archivo* archivo, int nuevo_tamanio_bytes, int tamanio_bloque);

// - recibe el conjunto de archivo y el nombre del archivo que quedara al final de la compactacion
void compactar_archivos(t_dictionary* archivos, char* nombre_archivo);
void compactar_archivos_usando_mas_memoria(t_dictionary* archivos, char* nombre_archivo);

// - setea en el metadato un entero en el key elegido y lo guarda
// - (el metadato debe estar inicializado) 
void set_key_metadata(t_config* metadato, char* key, int valor);

void hilo_habilitador(int milisegundos);
void habilitador_de_compactacion(void* args);

estado_interfaz escribir_archivo_test(char* nombre_archivo, uint32_t bytes, uint32_t puntero, void* data_a_escribir, uint32_t pid);
estado_interfaz leer_archivo_dial_test(char* nombre_archivo, uint32_t bytes, uint32_t puntero, uint32_t pid);

#endif