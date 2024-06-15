#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>

#include "cpu_config.h"
#include <logging/logging.h>
#include "generales.h"
#include <pthread.h>
#include "init.h"
#include <serializacion/serializacion.h>
#include <enum/enum.h>
#include <utils/utils.h>

t_log* logger = NULL;
t_cpu_config* config = NULL;

pthread_t hilo_dispatch;
pthread_t hilo_interrupt;

int conexion_memoria;
int server_cpu_dispatch_fd;
int server_cpu_interrupt_fd;

int tamanio_pagina_memoria;


int main(int argc, char* argv[]) {
    
    logger = iniciar_logger("cpu.log", "CPU");
    log_info(logger, "Iniciando Modulo CPU \n");
    
    config = init_cpu_config("cpu.config");

    init_cpu();

    conexion_memoria = crear_conexion(config->ip_memoria, config->puerto_memoria, "MEMORIA", logger);
    enviar_handshake(conexion_memoria, HANDSHAKE, "CPU", "MEMORIA", logger);
    envio_generico_op_code(conexion_memoria, SOLICITUD_TAMANIO_PAGINA);
    ignorar_op_code(conexion_memoria);
    tamanio_pagina_memoria = (int)recibo_generico_entero(conexion_memoria);

    server_cpu_dispatch_fd = iniciar_servidor("CPU DISPATCH", config->ip_cpu, config->puerto_escucha_dispatch, logger);
    log_info(logger, "CPU DISPATCH Escuchando Conexiones \n");
    pthread_create(&hilo_dispatch, NULL, (void*) escuchar_dispatch, NULL);

    server_cpu_interrupt_fd = iniciar_servidor("CPU INTERRUPT", config->ip_cpu, config->puerto_escucha_interrupt, logger);
    log_info(logger, "CPU INTERRUPT Escuchando Conexiones \n");
    pthread_create(&hilo_interrupt, NULL, (void*) escuchar_interrupt, NULL);
    
    pthread_join(hilo_dispatch, NULL);
    pthread_join(hilo_interrupt, NULL);
    
    liberar_cpu();
    liberar_semaforos();

    return 0;
}



/*
int main(int argc, char* argv[]) {
    
    logger = iniciar_logger("cpu.log", "CPU");
    log_info(logger, "Iniciando Modulo CPU \n");
    
    config = init_cpu_config("cpu.config");

    init_cpu();

    //uint32_t entero_h = 0x616C6F48;
    //uint32_t entero_d = 1634496328;
    //uint32_t entero_d = 1634496328;

    uint32_t entero = 0;
    size_t size = 3;
    
    char* aux = "pollsasasas";
    printf("Cadena test (%s)\n", aux);

    entero = cadena_a_valor_entero((void*) aux, size);
    printf("Entero (%u)\n", entero);

    char* cadena = valor_entero_a_cadena(entero, size);
    printf("Cadena almacenada (%s)\n", cadena);
    
    free(cadena);
    
    liberar_cpu();
    liberar_semaforos();

    return 0;
}
*/



