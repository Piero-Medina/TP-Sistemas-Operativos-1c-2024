#ifndef GENERALES_H_
#define GENERALES_H_

#include <commons/log.h>
#include <commons/collections/dictionary.h>
#include <stdbool.h>

#include "entradaSalida_config.h"
#include "bit_map_dialFs.h"

#include <enum/enum.h>

typedef struct{
    char* nombre; // nombre (sin ruta y con extension)
    char* path_archivo;
    char* path_metadato;
    int bloque_inicial;  // en bloque
    int tamanio_archivo; // en bytes        
}t_archivo;

// declarado en main.c
extern t_log* logger;
extern t_entradaSalida_config* config;

extern tipo_interfaz tipo_de_interfaz;

extern int conexion_kernel;
extern int conexion_memoria;

// declarado en init.c
extern bool procesar_conexion_en_ejecucion;

extern t_bitmap* bitmap;

extern char* path_bitmap;
extern char* path_bloques;
extern char* path_metadatos;
extern char* path_archivos;

extern t_dictionary* diccionario_de_archivos;

#endif 