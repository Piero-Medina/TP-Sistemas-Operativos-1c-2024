#ifndef IO_PENDIENTE_H
#define IO_PENDIENTE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#include <commons/collections/list.h>

#include <peticion_memoria/peticion_memoria.h>

typedef struct {
	uint32_t pid;
	char* nombre_interfaz;
	int operacion;
	bool ejecutando;
	char* parametro_string;
    t_list* peticiones_memoria; // (t_peticion_memoria*)
	uint32_t parametro_int_1;
	uint32_t parametro_int_2;
	uint32_t parametro_int_3;
	uint32_t parametro_int_4;
}t_io_pendiente;

// - se inicializa y se reserva memoria para un io_pendiente.
// - si param_string es NULL, no se agrega.
// - si peticiones es NULL, no se agrega.
t_io_pendiente* inicializar_io_pendiente(uint32_t pid, char* nombre_interfaz, int operacion, bool ejecutando, char* param_string, t_list* peticiones, uint32_t param_int_1, uint32_t param_int_2, uint32_t param_int_3, uint32_t param_int_4);

void imprimir_io_pendiente(t_io_pendiente* io);

t_io_pendiente* buscar_io_pendiente_por_pid_y_obtener(uint32_t pid, t_list* lista);
t_io_pendiente* buscar_io_pendiente_por_pid_y_remover(uint32_t pid, t_list* lista);
int posicion_de_io_pendiente_por_pid(uint32_t pid, t_list* lista);

void liberar_elemento_t_io_pendiente(void* elemento);

#endif