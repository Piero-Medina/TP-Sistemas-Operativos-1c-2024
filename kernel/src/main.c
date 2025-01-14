#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>

#include "kernel_config.h"
#include <logging/logging.h>
#include <conexiones/conexiones.h>
#include "generales.h"
#include <serializacion/serializacion.h>
#include <enum/enum.h>
#include "procesar_conexion.h"
#include "init.h"
#include <pthread.h>
#include "consola_interactiva.h"
#include <stdbool.h>

#include <utils/utils.h>

#include <commons/collections/list.h>
#include <commons/string.h>

t_log* logger;
t_kernel_config* config;

int conexion_cpu_dispatch;
int conexion_cpu_interrupt;
int conexion_memoria;
int server_fd;

pthread_t hilo_server_kernel;
pthread_t hilo_cpu_dispatch;


int main(int argc, char* argv[]) {

    if(argc != 2){
        printf("Error: Se esperan un parámetro\n");
        printf("%s <path_archivo_configuracion>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *path_archivo_configuracion = argv[1];

    logger = iniciar_logger_oculto("kernel.log", "KERNEL", LOG_LEVEL_INFO);
    log_info(logger, "Iniciando Modulo Kernel \n");
   
    config = init_kernel_config(path_archivo_configuracion);

    init_kernel();

    conexion_cpu_dispatch = crear_conexion(config->ip_cpu, config->puerto_cpu_dispatch, "CPU_DISPATCH" , logger);
    enviar_handshake(conexion_cpu_dispatch, HANDSHAKE, "KERNEL", "CPU_DISPATCH", logger);
    pthread_create(&hilo_cpu_dispatch, NULL, (void*) procesar_conexion_cpu_dispatch, NULL);
    pthread_detach(hilo_cpu_dispatch);

    conexion_cpu_interrupt = crear_conexion(config->ip_cpu, config->puerto_cpu_interrupt, "CPU_INTERRUPT", logger);
    enviar_handshake(conexion_cpu_interrupt, HANDSHAKE, "KERNEL", "CPU_INTERRUPT", logger);

    conexion_memoria = crear_conexion(config->ip_memoria, config->puerto_memoria, "MEMORIA", logger);
    enviar_handshake(conexion_memoria, HANDSHAKE, "KERNEL", "MEMORIA", logger);

    server_fd = iniciar_servidor("KERNEL", config->ip_kernel, config->puerto_kernel, logger);
    log_info(logger, "KERNEL Escuchando Conexiones IO\n");
    pthread_create(&hilo_server_kernel, NULL, (void*) escuchar_kernel, NULL);
    pthread_detach(hilo_server_kernel);

    consola_interactiva();
    
    liberar_kernel();
    
    return 0;
}