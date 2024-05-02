#include "init.h"

bool procesar_conexion_en_ejecucion;
bool desalojo;

sem_t sem_test;
sem_t mutex_desalojo;

void init_cpu(void){
    signal(SIGINT, sigint_handler);
    
    procesar_conexion_en_ejecucion = true;
    desalojo = false;

    init_semaforos();
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

    liberar_semaforos();

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
}