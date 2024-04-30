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

///////////////////////////////////////// WRAPPERS //////////////////////////////////////
void avisar_nuevo_proceso_memoria(int conexion_memoria, int pid, char* path_intrucciones);
void avisar_desalojo_a_cpu(int conexion_cpu, int op_code, char* motivo);
void perdir_intruccion_a_memoria(int conexion, int op_code, int pid, int pc);
/////////////////////////////////////////////////////////////////////////////////////////

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

// envio y recibo genericode un op_code y entero
void envio_generico_entero(int conexion, int op_code, int entero);
int recibo_generico_entero(int conexion);

// envio y recibo generico de un op_code y dos enteros
void envio_generico_doble_entero(int conexion, int op_code, int entero1, int entero2);
void recibo_generico_doble_entero(int conexion, int* entero1, int* entero2);

// funciones para el enviar y recibir un pcb por red
void enviar_pcb(int conexion, t_PCB* pcb, int codigo_operacion);
t_PCB* recibir_pcb(int conexion);
t_buffer* serializar_pcb(t_PCB* pcb);
t_PCB* deserializar_pcb(t_buffer* buffer);

// funciones necesarias para serializar una instruccion
void enviar_instruccion(int conexion, t_instruccion* instruccion, int codigo_operacion);
t_instruccion* recibir_instruccion(int conexion);
t_buffer* serializar_instruccion(t_instruccion* instruccion);
t_instruccion* deserializar_instruccion(t_buffer* buffer);

// funciones necesarias para la serializacion y deserializacion de una lista de string
t_buffer* serializar_lista_de_string(t_list* lista_de_string);
t_list* deserializar_lista_de_string(t_buffer* buffer);
int tamanio_serializable_lista_de_string(t_list* lista_de_string);

// por si solo se lo quiere agregar al buffer porque se va agregas mas datos, avanza el offset
void buffer_add_list_string(t_buffer* buffer, t_list* lista_de_string, int size_lista);
// lee una lista del buffer y avanza el offset
t_list* buffer_read_list_string(t_buffer* buffer);

#endif