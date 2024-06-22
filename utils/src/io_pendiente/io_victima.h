#ifndef IO_VICTIMA_H
#define IO_VICTIMA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#include <commons/collections/list.h>

typedef enum{
    USUARIO_CONSOLA,
    INTERFAZ_DESCONECTADA
}tipo_victima;

typedef struct {
	uint32_t pid;
	tipo_victima motivo_victima;
    char* nombre_interfaz;
}t_io_victima;

t_io_victima* crear_t_io_victima(uint32_t pid, tipo_victima motivo, char* nombre_interfaz);

int posicion_de_io_victima_por_pid(uint32_t pid, t_list* lista);
t_io_victima* buscar_io_victima_por_pid_y_obtener(uint32_t pid, t_list* lista);
t_io_victima* buscar_io_victima_por_pid_y_remover(uint32_t pid, t_list* lista);

void liberar_elemento_t_io_victima(void* elemento); 

#endif