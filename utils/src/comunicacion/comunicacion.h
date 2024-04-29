#ifndef COMUNICACION_H
#define COMUNICACION_H

#include <enum/enum.h>
#include <serializacion/serializacion.h>
#include <stdlib.h>
#include <stdint.h> 
#include <string.h>
#include <sys/socket.h>

#include <commons/collections/list.h>

#include <pcb/pcb.h>
#include <instruccion/instruccion.h>

void avisar_nuevo_proceso_memoria(int conexion_memoria, int pid, char* path_intrucciones);

void avisar_desalojo_a_cpu(int conexion_cpu, int op_code, char* motivo);

// envio generico de un op_code
void envio_generico_op_code(int conexion, int op_code);
int recibo_generico_op_code(int conexion);
void validar_respuesta_op_code(int conexion, int op_code_esperado, t_log* logger);

// envio y recibo generico de un op_code, entero y un string
void envio_generico_entero_y_string(int conexion, int op_code, int entero, char* string);
void recibir_generico_entero_string(int conexion, int* entero, char** string);

// envio y recibo generico de un op_code y string
void envio_generico_string(int conexion, int op_code, char* string);
void recibir_generico_string(int conexion, char** string);

// funciones para el enviar y recibir un pcb por red
void enviar_pcb(int conexion, t_PCB* pcb, int codigo_operacion);
t_PCB* recibir_pcb(int conexion);
t_buffer* serializar_pcb(t_PCB* pcb);
t_PCB* deserializar_pcb(t_buffer* buffer);


// funciones necesarias para serializar una instruccion
t_buffer* serializar_intruccion(t_instruccion* instruccion);
t_instruccion* deserializar_intruccion(t_buffer* buffer);

// funciones necesarias para la serializacion y deserializacion de una lista de string
t_buffer* serializar_lista_de_string(t_list* lista_de_string);
t_list* deserializar_lista_de_string(t_buffer* buffer);
int tamanio_serializable_lista_de_string(t_list* lista_de_string);
// por si solo se lo quiere agregar al buffer porque se va agregas mas datos, avanza el offset
void buffer_add_list_string(t_buffer* buffer, t_list* lista_de_string, int size_lista);
// lee una lista del buffer y avanza el offset
t_list* buffer_read_list_string(t_buffer* buffer);

#endif