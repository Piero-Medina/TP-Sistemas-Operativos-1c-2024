#ifndef COMUNICACION_H
#define COMUNICACION_H

#include <enum/enum.h>
#include <serializacion/serializacion.h>
#include <stdlib.h>
#include <stdint.h> 
#include <string.h>
#include <sys/socket.h>

#include <pcb/pcb.h>

void avisar_nuevo_proceso_memoria(int conexion_memoria, int pid, char* path_intrucciones);

// funciones para el enviar y recibir un pcb por red
void enviar_pcb(int conexion, t_PCB* pcb, int codigo_operacion);
t_PCB* recibir_pcb(int conexion);
t_buffer* serializar_pcb(t_PCB* pcb);
t_PCB* deserializar_pcb(t_buffer* buffer);

#endif