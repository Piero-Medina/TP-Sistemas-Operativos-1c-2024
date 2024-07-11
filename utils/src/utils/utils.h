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

#define MAX_SIZE 500

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



// - convierte una cadena a su equivalente en valor entero (solo permite 1 o 4 bytes).
// - no importa el largo de la cadena, este solo copiara 1 o 4 bytes.
uint32_t cadena_a_valor_entero(void* cadena, size_t bytes);

// - convierte un valor entero a su equivalente en cadena (solo permite 1 o 4 bytes)
// - Al convertir en cadena le agrega el (`'\0'`) al final, por lo que la cadena
//   resultante terminara teniendo un 1 byte mas de tamanio.
// - (Liberar memoria)
char* valor_entero_a_cadena(uint32_t entero, size_t bytes);

// - devuelve una cadena nueva con memoria asignada a partir de un entero.
// - (liberar memoria)
char* valor_entero_a_cadena_nueva(uint32_t entero, size_t bytes); 

/// @brief Convierte un bloque de datos en una cadena de caracteres.
/// Esta función reasigna memoria para agregar un carácter nulo (`'\0'`) al final
/// del bloque de datos, convirtiéndolo en una cadena de caracteres.
/// 
/// @param data Un puntero al bloque de datos que se va a convertir. Debe apuntar
///             a una memoria previamente asignada con `malloc` o similar.
/// @param bytes El tamaño del bloque de datos en bytes.
/// @return Un puntero a la cadena de caracteres resultante, o `NULL` si `realloc` falla.
/// 
/// @note Si `realloc` falla, se imprime un mensaje de error y se retorna `NULL`.
/// @note La memoria original apuntada por `data` no debe ser liberada directamente
///       después de llamar a esta función. En su lugar, se debe usar el puntero 
///       retornado por esta función para liberar la memoria con `free`.
/// @note - (Liberar el puntero retornado en caso de exito realloc)
/// @note - (Liberar data retornado en caso de fallar realloc).        
char* convertir_a_cadena(void* data, size_t bytes);

// - devuelve una cadena nueva con memoria asignada a partir de una data.
// - (liberar memoria)
char* convertir_a_cadena_nueva(void* data, size_t bytes);

#endif