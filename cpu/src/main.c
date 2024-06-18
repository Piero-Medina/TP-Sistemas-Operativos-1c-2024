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
    tamanio_pagina_memoria = (int)recibir_generico_entero(conexion_memoria);

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


/* TEST MMU MULTIPAGINAS
int main(int argc, char* argv[]) {
    
    logger = iniciar_logger("cpu.log", "CPU");
    log_info(logger, "Iniciando Modulo CPU \n");
    
    config = init_cpu_config("cpu.config");

    init_cpu();

    tamanio_pagina_memoria = 32;

    int direccion_logica = 31;
    uint32_t pid = 1;
    uint32_t tamanio_bytes = 96;

    // Agregamos algunas entradas para llenar la TLB
    agregar_entrada_tlb(tlb, FIFO, pid, 0, 0); // pid=1, pagina=0, marco=0
    agregar_entrada_tlb(tlb, FIFO, pid, 1, 1); // pid=2, pagina=1, marco=1
    agregar_entrada_tlb(tlb, FIFO, pid, 2, 2); // pid=3, pagina=2, marco=2
    agregar_entrada_tlb(tlb, FIFO, pid, 3, 3); // pid=4, pagina=3, marco=3

    // Mostramos el estado final de la TLB
    printf("\nEstado final de la TLB:\n");
    imprimir_lista_entrada_tlb_sin_tiempo(tlb);
    
    printf("\nUSANDO MMU\n\n");
    t_list* direcciones_fisicas = list_create();
    int estado = MMU(direccion_logica, tamanio_pagina_memoria, pid, (uint32_t)tamanio_bytes, direcciones_fisicas);

    if(estado == MMU_OK){
        printf("\nMMU_OK\n");
    }

    printf("\nEstado final de las Peticiones de memoria:\n");
    imprimir_lista_peticion_memoria(direcciones_fisicas);
    
    liberar_lista_de_peticiones_memoria(direcciones_fisicas);

    liberar_cpu();
    liberar_semaforos();

    return 0;
}
*/




