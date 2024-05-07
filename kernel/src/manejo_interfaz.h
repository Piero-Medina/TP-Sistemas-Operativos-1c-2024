#ifndef MANEJO_INTERFAZ_H_
#define MANEJO_INTERFAZ_H_

#include <stdio.h>
#include <stdlib.h>
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
t_io_pendiente* inicializar_io_pendiente(int pid, int operacion, bool interfaz_ocupada, char* param_string, int param_int_1, int param_int_2, int param_int_3, int param_int_4);

// verifica que interfaces estan libres para procesar una IO
void manejador_de_procesos_pendientes_io(void* arg);

// manda a procesar un paticion de io a una interfaz, dentro discrimina segun la operacion
void mandar_a_procesar_io(int conexion_interfaz, t_io_pendiente* pendiente);
void solicitar_IO_GEN_SLEEP(int conexion_interfaz, t_io_pendiente* pendiente);

// busca un pcb mediante el pid en una lista y lo remueve
t_PCB* buscar_pcb_por_pid_y_remover(int pid, t_list* lista);
int posicion_de_pcb_por_pid(int pid, t_list* lista);

#endif 