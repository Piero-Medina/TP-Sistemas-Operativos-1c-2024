#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// creacion de directorios
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <pthread.h>

#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <commons/collections/node.h>
#include <commons/collections/dictionary.h>

#include <pcb/pcb.h>

#define MAX_SIZE 80

// abre un archivo en modo lectura
FILE* leer_archivo(char *path);
void cerrar_archivo(FILE* archivo);

char* string_aplanar_PID(t_list* lista);

// funciones usadas para simular IO por el momento
void hilo_dormir_milisegundos(int milisegundos);
void mimir_milisegundos(void* args);
void sleep_ms(int milisegundos);

// funcion que concatena una ruta, liberar memoria
char* concatenar_ruta(char* base, char* nombre_archivo);

// funcion que remueve la extension del nombre de un archivo, liberar memoria
char* remover_extension(const char* nombre_archivo);

// funcion que concatena el nombre y la extension, liberar memoria
char* concatenar_nombre_y_extension(char* nombre, char* extension);

// funcion que verifica la existencia de un directorio
// 0-> diretorio existe | 1 -> directorio creado | -1 error
int verificar_directorio(char* ruta);

// cantidad de bloques ocupados (redondeando hacia arriba)
int cantidad_de_bloques_ocupados(int bytes, int tamanio_bloque);

// cantidad de bytes libres sin usar en el ultimo bloque
int bytes_sobrantes_de_ultimo_bloque(int bytes, int tamanio_bloque);

// convierte un entero a string, liberar memoria
char* entero_a_string(int entero);

int calcular_bloques_necesarios(int bytes, int tamanio_bloque);

char* eliminar_caracteres_finales(const char* buffer, char caracter);
char* eliminar_caracteres(const char* buffer, char caracter);

// - funcion para remover el primer char de un string (liberar memoria)
// - (La usamos porque los path /scripts_kernel no las pueden leer los modulos, pero si scripts_kernel)
// - /scripts_kernel/PRUEBA_PLANI -> fun(string, '/') -> scripts_kernel/PRUEBA_PLANI
char* remover_primer_char_si_machea(const char* str, char char_to_remove);

#endif