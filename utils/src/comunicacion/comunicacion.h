#ifndef COMUNICACION_H
#define COMUNICACION_H

#include <enum/enum.h>
#include <serializacion/serializacion.h>
#include <stdlib.h>
#include <stdint.h> 
#include <string.h>
#include <sys/socket.h>

void avisar_nuevo_proceso_memoria(int conexion_memoria, int PID, char* path_intrucciones);

#endif