#include "init.h"

bool procesar_conexion_en_ejecucion;
int contador_pid;

sem_t mutex_conexion_memoria;
sem_t sem_grado_multiprogramacion;
sem_t mutex_pid;
sem_t mutex_cola_new;

t_queue* cola_new;

void init_kernel(void){
    signal(SIGINT, sigint_handler);
    procesar_conexion_en_ejecucion = true;
    contador_pid = 0;
    init_semaforos();
    init_colas();
}

void sigint_handler(int signum){
    printf("\n Finalizando el servidor por señal... \n");
    
    // finalizamos la ejecucion de cualquier hilo (que usa procesar_conexion_kernel) para que libere recursos correctamente
    procesar_conexion_en_ejecucion = false; 
    liberar_kernel();
    liberar_semaforos();
    liberar_colas();

    exit(EXIT_SUCCESS);
}

void escuchar_kernel(void *arg){
    while(server_listen(logger, "KERNEL", server_fd, (void*)procesar_conexion_io));
}

void liberar_kernel(void){
    log_destroy(logger);
    liberar_kernel_config(config);
    liberar_conexion(conexion_cpu_dispatch);
    liberar_conexion(conexion_cpu_interrupt);
    liberar_conexion(conexion_memoria);
    liberar_conexion(server_fd);
}

void init_semaforos(void){
    sem_init(&mutex_conexion_memoria, 0, 1);
    sem_init(&sem_grado_multiprogramacion, 0, config->grado_max_multiprogramacion);
    sem_init(&mutex_pid, 0, 1);
    sem_init(&mutex_cola_new, 0, 1);
}

void liberar_semaforos(void){
    sem_destroy(&mutex_conexion_memoria);
    sem_destroy(&sem_grado_multiprogramacion);
    sem_destroy(&mutex_pid);
    sem_destroy(&mutex_cola_new);
}

void init_colas(void){
    cola_new = queue_create();
}

void liberar_colas(void){
    queue_destroy_and_destroy_elements(cola_new, (void*)liberar_elemento_pcb);    
}

void liberar_elemento_pcb(void* elemento){
    t_PCB* tmp = (t_PCB*) elemento;
    liberar_PCB(tmp);
}


