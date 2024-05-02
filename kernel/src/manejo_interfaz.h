#ifndef MANEJO_INTERFAZ_H_
#define MANEJO_INTERFAZ_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/dictionary.h>

#include <enum/enum.h>
#include <comunicacion/comunicacion.h>
#include <instruccion/instruccion.h>

#include "generales.h"
#include "init.h"

// agregamos una t_interfaz al diccionario (diccionario interfaces disponibles en el kernel)
void agregar_interfaz(t_dictionary* interfaces, char* nombre_interfaz, int conexion, int tipo_interfaz);
// quitamos una t_interfaz al diccionario (diccionario interfaces disponibles en el kernel)
void quitar_interfaz(t_dictionary* interfaces, char* nombre_interfaz);


void verificar_tipo_interfaz(int conexion, int tipo_de_interfaz, char* nombre_interfaz);

void verificar_operacion_generica(int conexion, char* nombre_interfaz, int operacion);
void verificar_operacion_stdin(int conexion, char* nombre_interfaz, int operacion);
void verificar_operacion_stdout(int conexion, char* nombre_interfaz, int operacion);
void verificar_operacion_dialFs(int conexion, char* nombre_interfaz, int operacion);
#endif 