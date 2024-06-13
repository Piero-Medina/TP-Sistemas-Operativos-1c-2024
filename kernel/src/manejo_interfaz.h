#ifndef MANEJO_INTERFAZ_H_
#define MANEJO_INTERFAZ_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <commons/log.h>

#include <enum/enum.h>
#include <comunicacion/comunicacion.h>
#include <instruccion/instruccion.h>
#include <pcb/pcb.h>

#include "generales.h"
#include "init.h"
#include "planificacion.h"

// agregamos una t_interfaz al diccionario (diccionario interfaces disponibles en el kernel)
void agregar_interfaz(t_dictionary* interfaces, char* nombre_interfaz, int conexion, int tipo_interfaz);
// quitamos una t_interfaz al diccionario (diccionario interfaces disponibles en el kernel)
void quitar_interfaz(t_dictionary* interfaces, char* nombre_interfaz);

// recibe el tipo de interfaz y el nombre de la interfaz
void verificar_tipo_interfaz(int conexion, t_PCB* pcb);

// verifican el tipo de operacion que se quiere realizar
void verificar_operacion_generica(int conexion, char* nombre_interfaz, t_PCB* pcb);
void verificar_operacion_stdin(int conexion, char* nombre_interfaz, int operacion);
void verificar_operacion_stdout(int conexion, char* nombre_interfaz, int operacion);
void verificar_operacion_dialFs(int conexion, char* nombre_interfaz, int operacion);

// valida si el tipo de interfaz acepta tal operacion
bool validar_operacion(tipo_interfaz tipo_de_interfaz, t_identificador operacion);

// se inicializa y se reserva memoria para un io_pendiente
t_io_pendiente* inicializar_io_pendiente(uint32_t pid, int operacion, bool interfaz_ocupada, char* param_string, uint32_t param_int_1, uint32_t param_int_2, uint32_t param_int_3, uint32_t param_int_4);

// verifica que interfaces estan libres para procesar una IO
void manejador_de_procesos_pendientes_io(void* arg);

// manda a procesar un paticion de io a una interfaz, dentro discrimina segun la operacion
void mandar_a_procesar_io(int conexion_interfaz, t_io_pendiente* pendiente);
void solicitar_IO_GEN_SLEEP(int conexion_interfaz, t_io_pendiente* pendiente);

// - Busca un pcb mediante el pid en una lista y lo remueve
// - Retorna NULL en caso de no encontrar el pid en la lista
t_PCB* buscar_pcb_por_pid_y_remover(int pid, t_list* lista);
// - Busca un pcb mediante el pid en una lista y lo obtiene
// - Retorna NULL en caso de no encontrar el pid en la lista
t_PCB* buscar_pcb_por_pid_y_obtener(int pid, t_list* lista);
// - Busca un pcb mediante el pid en una lista y devuelve su posicion en la lista
// - Retorna (-1) en caso de no encontrar el pid en la lista
int posicion_de_pcb_por_pid(int pid, t_list* lista);


t_io_pendiente* buscar_io_pendiente_por_pid_y_obtener(uint32_t pid, t_list* lista);
t_io_pendiente* buscar_io_pendiente_por_pid_y_remover(uint32_t pid, t_list* lista);
int posicion_de_io_pendiente_por_pid(uint32_t pid, t_list* lista);

// - retorna true si el pid victima se encuentra entre los pendientes de eliminacion, lo remueve y libera la memoria asociada
// - Dentro garantizamos Mutua Exclusion para t_list* victimas_pendientes_io;
bool pid_pendiente_finalizacion(uint32_t pid, t_list* lista);

#endif 