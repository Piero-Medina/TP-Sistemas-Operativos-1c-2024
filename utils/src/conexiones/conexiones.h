#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commons/log.h>

#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include <pthread.h>

// estructura para procesar a un cliente
typedef struct {
    t_log* log;
    int fd_cliente;
    char* server_name;
}t_procesar_conexion_args;

// Crear un socket cliente para conectarse - termina el proceso si no pudo conextarse
int crear_conexion(char *ip, char* puerto, char* nombreDelProceso, t_log* logger);

// Crear un socket servidor para recibir clientes
int iniciar_servidor(char* name, char* ip, char* puerto, t_log* logger);

// Escuchar a nuevos clientes, crear un hilo si un cliente se conecta, recibe la funcion que va a tomar el hilo
int server_listen(t_log* logger, char* server_name, int server_socket, void(*func_procesar_conexion)(void*));

// acepta clientes
int esperar_clientes(t_log* logger, const char* name, int socket_servidor);

// funcion template (que recibe el hilo), modificar segun el uso que se le de
void procesar_conexion_template(void *args);

// recibe un codigo de operacion 
int recibir_operacion(int socket_cliente);

// libera una conexion 
void liberar_conexion(int socket_cliente);

#endif