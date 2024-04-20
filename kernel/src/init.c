#include "init.h"

bool procesar_conexion_en_ejecucion;
int contador_pid;
algoritmo algoritmo_elegido;

sem_t mutex_conexion_memoria;
sem_t mutex_conexion_cpu_dispatch;

sem_t sem_grado_multiprogramacion;
sem_t mutex_pid;
sem_t sem_procesos_esperando_en_new;
sem_t sem_procesos_esperando_en_ready;

sem_t sem_cpu_disponible;

sem_t mutex_cola_new;
sem_t mutex_cola_ready;
sem_t mutex_cola_execute;

t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_execute;

t_dictionary* recursos;

void init_kernel(void){
    signal(SIGINT, sigint_handler);
    procesar_conexion_en_ejecucion = true;
    contador_pid = 0;
    
    algorimo_elegido();
    init_recursos();
    init_semaforos();
    init_colas();
}

void sigint_handler(int signum){
    printf("\n Finalizando el servidor por señal... \n");
    
    // finalizamos la ejecucion de cualquier hilo (que usa procesar_conexion_kernel) para que libere recursos correctamente
    procesar_conexion_en_ejecucion = false; 
    liberar_kernel();

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

    liberar_semaforos();
    liberar_colas();
    liberar_recursos();
}

void init_semaforos(void){
    sem_init(&mutex_conexion_memoria, 0, 1);
    sem_init(&mutex_conexion_cpu_dispatch, 0, 1);

    sem_init(&sem_grado_multiprogramacion, 0, config->grado_max_multiprogramacion); // contador
    sem_init(&mutex_pid, 0, 1);
    sem_init(&sem_procesos_esperando_en_new, 0, 0); // contador
    sem_init(&sem_procesos_esperando_en_ready, 0, 0); // contador

    sem_init(&sem_cpu_disponible, 0, 1); // Binario "ponele" (solo un cpu)

    sem_init(&mutex_cola_new, 0, 1);
    sem_init(&mutex_cola_ready, 0, 1);
    sem_init(&mutex_cola_execute, 0, 1);
}

void liberar_semaforos(void){
    sem_destroy(&mutex_conexion_memoria);
    sem_destroy(&mutex_conexion_cpu_dispatch);

    sem_destroy(&sem_grado_multiprogramacion);
    sem_destroy(&mutex_pid);
    sem_destroy(&sem_procesos_esperando_en_new);
    sem_destroy(&sem_procesos_esperando_en_ready);

    sem_destroy(&sem_cpu_disponible);

    sem_destroy(&mutex_cola_new);
    sem_destroy(&mutex_cola_ready);
    sem_destroy(&mutex_cola_execute);
}

void init_colas(void){
    cola_new = queue_create();
    cola_ready  = queue_create();
    cola_execute  = queue_create();
}

void liberar_colas(void){
    queue_destroy_and_destroy_elements(cola_new, (void*)liberar_elemento_pcb); 
    queue_destroy_and_destroy_elements(cola_ready, (void*)liberar_elemento_pcb);
    queue_destroy_and_destroy_elements(cola_execute, (void*)liberar_elemento_pcb);      
}

void liberar_elemento_pcb(void* elemento){
    t_PCB* tmp = (t_PCB*) elemento;
    liberar_PCB(tmp);
}


void algorimo_elegido(void){
    if(strcmp(config->algoritmo_planificacion, "FIFO") == 0) {
        algoritmo_elegido = FIFO;
        log_info(logger, "Se eligio el algoritmo de planificacion FIFO");
    }

    if(strcmp(config->algoritmo_planificacion,"RR") == 0) {
        algoritmo_elegido = RR;
        log_info(logger, "Se eligio el algoritmo de planificacion RR (Round Robin)");
    }

    if(strcmp(config->algoritmo_planificacion,"VRR") == 0) {
        algoritmo_elegido = VRR;
        log_info(logger, "Se eligio el algoritmo de planificacion VRR (Virtual Round Robin)");
    }
}

void init_recursos(void){
	recursos = dictionary_create();

	if( !string_array_is_empty(config->recursos) ){
		int cant_recursos = string_array_size(config->recursos);

		for (int i = 0; i < cant_recursos; i++){
			char* nombre_recurso = config->recursos[i];

			t_recurso* tmp = malloc(sizeof(t_recurso));
			tmp->cola_recurso = queue_create();
			tmp->instancias = atoi(config->instancias_recursos[i]);

			dictionary_put(recursos, nombre_recurso, (void*)tmp);
		}
		log_info(logger, "Se Tiene Recursos \n");

	}
	else{
		log_info(logger, "No se Tiene Recursos \n");
	}
}

void liberar_recursos(void){
	dictionary_destroy_and_destroy_elements(recursos, liberar_elemento_recurso);
}

void liberar_elemento_recurso(void* elemento){
    t_recurso* tmp = (t_recurso*) elemento;
    queue_destroy_and_destroy_elements(tmp->cola_recurso, (void*)liberar_elemento_pcb);
    free(tmp);
}