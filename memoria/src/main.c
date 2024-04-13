#include <stdlib.h>
#include <stdio.h>

#include <logging/logging.h>
#include <commons/log.h>
#include "memoria_config.h"
#include <conexiones/conexiones.h>
#include "procesar_conexion.h"
#include "init.h"

t_log* logger;
t_memoria_config* config;

int server_fd;

int main(int argc, char* argv[]) {

    init_memoria();
    
    logger = iniciar_logger("memoria.log", "MEMORIA");
    log_info(logger, "Iniciando Modulo MEMORIA \n");

    config = init_memoria_config("memoria.config");
    
    server_fd = iniciar_servidor("MEMORIA", config->ip_memoria, config->puerto_escucha_memoria, logger);
    log_info(logger, "MEMORIA Escuchando Conexiones \n");
    while (server_listen(logger, "MEMORIA", server_fd, (void*) procesar_conexion_general));

    liberar_memoria();

    return 0;
    
}
