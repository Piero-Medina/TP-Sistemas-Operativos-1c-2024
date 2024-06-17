#ifndef PETICION_MEMORIA_H
#define PETICION_MEMORIA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include <commons/collections/list.h>

#include <serializacion/serializacion.h>

typedef struct {
    uint32_t base;
    uint32_t direccion_fisica;
    uint32_t bytes;
}t_peticion_memoria;

t_peticion_memoria* crear_peticion_memoria(uint32_t base, uint32_t direccion_fisica, uint32_t bytes);
void imprimir_peticion_memoria(t_peticion_memoria* peticion);
void liberar_elemento_peticion_memoria(void* elemento);
void liberar_lista_de_peticiones_memoria(t_list* lista);

// - el tamanio en bytes de la lista se calcula antes para crear el buffer.
// - calculo = sizeof(uint32_t); -> espacio para el tamanio en bytes de el largo de la lista
// - calculo += size_lista * (3 * sizeof(t_peticion_memoria)); -> espacio para el tamanio en bytes de la lista 
void buffer_add_list_t_peticion_memoria(t_buffer* buffer, t_list* lista, uint32_t size_lista);
t_list* buffer_read_list_t_peticion_memoria(t_buffer* buffer);

t_buffer* serializar_lista_de_t_peticion_memoria(t_list* lista);
t_list* deserializar_lista_de_t_peticion_memoria(t_buffer* buffer);

#endif 