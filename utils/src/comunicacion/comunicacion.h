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

void avisar_desalojo_a_cpu(int conexion_cpu, int op_code, char* motivo);

// envio generico de un op_code
void envio_generico_op_code(int conexion, int op_code);

// envio generico de un op_code, entero y un string
void envio_generico_entero_y_string(int conexion, int op_code, int entero, char* string);

// envio generico de un op_code y string
void envio_generico_string(int conexion, int op_code, char* string);

// funciones para el enviar y recibir un pcb por red
void enviar_pcb(int conexion, t_PCB* pcb, int codigo_operacion);
t_PCB* recibir_pcb(int conexion);
t_buffer* serializar_pcb(t_PCB* pcb);
t_PCB* deserializar_pcb(t_buffer* buffer);

#endif