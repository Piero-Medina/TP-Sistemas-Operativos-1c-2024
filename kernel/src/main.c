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

t_log* logger = NULL;
t_kernel_config* config = NULL;

int conexion_cpu_dispatch;
int conexion_cpu_interrupt;
int conexion_memoria;
int server_fd;

int main(int argc, char* argv[]) {

    init_kernel();

    logger = iniciar_logger("kernel.log", "KERNEL");
    log_info(logger, "Iniciando Modulo Kernel \n");
   
    config = init_kernel_config("kernel.config");

    conexion_cpu_dispatch = crear_conexion(config->ip_cpu, config->puerto_cpu_dispatch, "CPU_DISPATCH" , logger);
    enviar_handshake(conexion_cpu_dispatch, HANDSHAKE, "KERNEL", "CPU_DISPATCH", logger);

    conexion_cpu_interrupt = crear_conexion(config->ip_cpu, config->puerto_cpu_interrupt, "CPU_INTERRUPT", logger);
    enviar_handshake(conexion_cpu_interrupt, HANDSHAKE, "KERNEL", "CPU_INTERRUPT", logger);

    conexion_memoria = crear_conexion(config->ip_memoria, config->puerto_memoria, "MEMORIA", logger);
    enviar_handshake(conexion_memoria, HANDSHAKE, "KERNEL", "MEMORIA", logger);

    server_fd = iniciar_servidor("KERNEL", config->ip_kernel, config->puerto_kernel, logger);
    log_info(logger, "KERNEL Escuchando Conexiones \n");
    while(server_listen(logger, "KERNEL", server_fd, (void*)procesar_conexion_io));
    
    liberar_kernel();

    return 0;
}
