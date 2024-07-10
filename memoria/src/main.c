#include <stdlib.h>
#include <stdio.h>

#include <logging/logging.h>
#include <commons/log.h>
#include "memoria_config.h"
#include <conexiones/conexiones.h>
#include "procesar_conexion.h"
#include "init.h"

#include "peticiones_kernel.h"

t_log* logger;
t_memoria_config* config;

int server_fd;

int main(int argc, char* argv[]) {
    
    logger = iniciar_logger("memoria.log", "MEMORIA");
    log_info(logger, "Iniciando Modulo MEMORIA \n");

    config = init_memoria_config("memoria.config");

    init_memoria();
    
    server_fd = iniciar_servidor("MEMORIA", config->ip_memoria, config->puerto_escucha_memoria, logger);
    log_info(logger, "MEMORIA Escuchando Conexiones \n");
    while (server_listen(logger, "MEMORIA", server_fd, (void*) procesar_conexion_general));

    liberar_memoria();

    return 0; 
}

/* Test Bruno
int main(int argc, char* argv[]) {

    init_memoria();
    
    logger = iniciar_logger("memoria.log", "MEMORIA");
    log_info(logger, "Iniciando Modulo MEMORIA \n");

    config = init_memoria_config("memoria.config");
    
    //inicializo la memoria
    
    t_memoria_paginada* memoria = inicializar_memoria_paginada(config->tam_memoria,config->tam_pagina,logger);
    if(memoria == NULL){
        log_error(logger, "no se pudo asignar alguno de los valores necesarios para la memoria");
        return -4;
    }
    t_proceso proceso;
    proceso.instrucciones = list_create();
    proceso.marcos = list_create();
    proceso.pid = 1;
    
    //Incializo el servidor
    server_fd = iniciar_servidor("MEMORIA", config->ip_memoria, config->puerto_escucha_memoria, logger);
    log_info(logger, "MEMORIA Escuchando Conexiones \n");
    while (server_listen(logger, "MEMORIA", server_fd, (void*) procesar_conexion_general));
    
    
    log_info(logger, "asigno marcos al proceso: \n");
    if(asignar_paginas_a_proceso(&proceso, memoria,logger))
        log_info(logger,"asignada");
       
    //Libero memoria
    //libera_espacio_memoria(memoria,logger);
    liberar_memoria();
    
    return 0;
}
*/
