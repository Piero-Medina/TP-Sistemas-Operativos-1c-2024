#include "init.h"

bool procesar_conexion_en_ejecucion;
bool desalojo;
bool tlb_habilitada;

algoritmo algoritmo_elegido;

sem_t sem_test;
sem_t mutex_desalojo;

t_list* tlb;

void init_cpu(void){
    signal(SIGINT, sigint_handler);
    
    procesar_conexion_en_ejecucion = true;
    desalojo = false;

    init_semaforos();
    init_tlb();
    algorimo_usado();
}

void escuchar_dispatch(void *arg){
    sem_post(&sem_test);
    while(server_listen(logger, "CPU DISPATCH", server_cpu_dispatch_fd, (void*)procesar_conexion_kernel));
}

void escuchar_interrupt(void *arg){
    sem_wait(&sem_test);
    while(server_listen(logger, "CPU INTERRUPT", server_cpu_interrupt_fd, (void*)procesar_conexion_kernel));
}

void sigint_handler(int signum){
    printf("\n Finalizando el servidor por señal... \n");
    
    // finalizamos la ejecucion de cualquier hilo (que usa procesar_conexion_io) para que libere recursos correctamente
    procesar_conexion_en_ejecucion = false;
    
    log_info(logger, "¡Este proceso nunca debió existir! \n\n\n");
    
    liberar_cpu();

    exit(EXIT_SUCCESS);
}

void init_semaforos(void){
    sem_init(&sem_test, 0, 0); // contador - dar orden al inicio del servidor
    sem_init(&mutex_desalojo,0 ,1);   
}

void liberar_semaforos(void){
    sem_destroy(&sem_test);
    sem_destroy(&mutex_desalojo);
}

void liberar_cpu(void){
    log_destroy(logger);
    liberar_cpu_config(config);
    liberar_conexion(server_cpu_dispatch_fd);
    liberar_conexion(server_cpu_interrupt_fd);

    destruir_tlb(tlb);
    liberar_semaforos();
}

void init_tlb(void){
    if(config->cantidad_entradas_tlb != 0){
        log_info(logger, "TLB Habilitada - Entradas (%d) \n", config->cantidad_entradas_tlb);
        tlb_habilitada = true;
        tlb = crear_tlb(config->cantidad_entradas_tlb);
    }
    else{
        log_info(logger, "TLB Deshabilitada \n");
        tlb_habilitada = false;
    }
}

void algorimo_usado(void){
    if(tlb_habilitada){
        if(strcmp(config->algoritmo_tlb, "FIFO") == 0) {
            algoritmo_elegido = FIFO;
            log_info(logger, "Se eligio el algoritmo de reemplazo FIFO");
        }

        if(strcmp(config->algoritmo_tlb,"LRU") == 0) {
            algoritmo_elegido = LRU;
            log_info(logger, "Se eligio el algoritmo de reemplazo LRU");
        }
    }
}
