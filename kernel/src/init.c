#include "init.h"

bool procesar_conexion_en_ejecucion;
int contador_pid;
algoritmo algoritmo_elegido;
int grado_multiprogramacion_global;

bool proceso_en_ejecucion;

bool existe_recursos;

bool sistema_detenido;
bool stop_largo_plazo;
bool stop_corto_plazo;
bool stop_cpu_dispatch;
bool stop_io;

bool finalizacion_execute_afuera_kernel;
bool finalizacion_execute_dentro_kernel;

sem_t mutex_conexion_memoria;
sem_t mutex_conexion_cpu_dispatch;
sem_t mutex_conexion_cpu_interrupt;

sem_t sem_grado_multiprogramacion;
sem_t mutex_pid;
sem_t sem_procesos_esperando_en_new;
sem_t sem_procesos_esperando_en_ready;

sem_t sem_cpu_disponible;

sem_t mutex_cola_new;
sem_t mutex_cola_ready;
sem_t mutex_cola_ready_aux;
sem_t mutex_cola_execute;
sem_t mutex_cola_blocked;
sem_t mutex_cola_exit;

sem_t mutex_proceso_en_ejecucion;

sem_t mutex_diccionario_interfaces;

sem_t mutex_diccionario_recursos;

sem_t sem_peticiones_io_por_procesar;
sem_t sem_interfaz_io_libre;

sem_t sem_stop_largo_plazo;
sem_t sem_stop_corto_plazo;
sem_t sem_stop_cpu_dispatch;
sem_t sem_stop_io;

sem_t mutex_victimas_pendientes_io;
sem_t mutex_io_pendientes_ejecutando;

t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_ready_aux;
t_queue* cola_execute;
t_queue* cola_blocked;
t_queue* cola_exit;

t_dictionary* recursos;
t_dictionary* interfaces; // (t_interfaz*)

t_list* io_pendientes_ejecutando; // (t_io_pendiente* con ejecutando = true)
t_list* victimas_pendientes_io; // (t_io_victima*)
t_list* recursos_asignados;

pthread_t hilo_planificador_LP;
pthread_t hilo_planificador_CP;

void init_kernel(void){
    signal(SIGINT, sigint_handler);
    procesar_conexion_en_ejecucion = true;
    contador_pid = 1;
    grado_multiprogramacion_global = config->grado_max_multiprogramacion;
    proceso_en_ejecucion = false;

    sistema_detenido = false;
    stop_largo_plazo = false;
    stop_corto_plazo = false;
    stop_cpu_dispatch = false;
    stop_io = false;

    finalizacion_execute_afuera_kernel = false;
    finalizacion_execute_dentro_kernel = false;
    
    algorimo_elegido();
    init_diccionarios();
    init_semaforos();
    init_colas();
    init_planificadores();
    init_listas();
    init_manejador_de_procesos_io();
}

void sigint_handler(int signum){
    //printf("\n Finalizando el servidor por señal... \n");
    
    // finalizamos la ejecucion de cualquier hilo (que usa procesar_conexion_kernel) para que libere recursos correctamente
    procesar_conexion_en_ejecucion = false;
    log_info(logger, "¡Este proceso nunca debió existir! \n\n\n"); 
    liberar_kernel();

    exit(EXIT_SUCCESS);
}

void escuchar_kernel(void *arg){
    while(server_listen(logger, "CONEXION IO", server_fd, (void*)procesar_conexion_io));
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
    liberar_diccionarios();
    liberar_listas();
}

void init_semaforos(void){
    sem_init(&mutex_conexion_memoria, 0, 1);
    sem_init(&mutex_conexion_cpu_dispatch, 0, 1);
    sem_init(&mutex_conexion_cpu_interrupt, 0, 1);

    sem_init(&sem_grado_multiprogramacion, 0, config->grado_max_multiprogramacion); // contador
    sem_init(&mutex_pid, 0, 1);
    sem_init(&sem_procesos_esperando_en_new, 0, 0); // contador
    sem_init(&sem_procesos_esperando_en_ready, 0, 0); // contador

    sem_init(&sem_cpu_disponible, 0, 1); // Binario "ponele" (solo un cpu)

    sem_init(&mutex_cola_new, 0, 1);
    sem_init(&mutex_cola_ready, 0, 1);
    if(algoritmo_elegido == VRR) 
    sem_init(&mutex_cola_ready_aux, 0, 1);
    sem_init(&mutex_cola_execute, 0, 1);
    sem_init(&mutex_cola_blocked, 0, 1);
    sem_init(&mutex_cola_exit, 0, 1);

    sem_init(&mutex_proceso_en_ejecucion, 0, 1);

    sem_init(&mutex_diccionario_interfaces, 0, 1);

    sem_init(&mutex_diccionario_recursos, 0, 1);

    sem_init(&sem_peticiones_io_por_procesar, 0, 0); // Contador
    sem_init(&sem_interfaz_io_libre, 0, 0);          // Contador

    sem_init(&sem_stop_largo_plazo, 0, 1);
    sem_init(&sem_stop_corto_plazo, 0, 1);
    sem_init(&sem_stop_cpu_dispatch, 0, 1);
    sem_init(&sem_stop_io, 0, 1);

    sem_init(&mutex_victimas_pendientes_io, 0, 1);
    sem_init(&mutex_io_pendientes_ejecutando, 0, 1);
}

void liberar_semaforos(void){
    sem_destroy(&mutex_conexion_memoria);
    sem_destroy(&mutex_conexion_cpu_dispatch);
    sem_destroy(&mutex_conexion_cpu_interrupt);

    sem_destroy(&sem_grado_multiprogramacion);
    sem_destroy(&mutex_pid);
    sem_destroy(&sem_procesos_esperando_en_new);
    sem_destroy(&sem_procesos_esperando_en_ready);

    sem_destroy(&sem_cpu_disponible);

    sem_destroy(&mutex_cola_new);
    sem_destroy(&mutex_cola_ready);
    if(algoritmo_elegido == VRR) 
    sem_destroy(&mutex_cola_ready_aux);
    sem_destroy(&mutex_cola_execute);
    sem_destroy(&mutex_cola_blocked);
    sem_destroy(&mutex_cola_exit);

    sem_destroy(&mutex_proceso_en_ejecucion);

    sem_destroy(&mutex_diccionario_interfaces);

    sem_destroy(&mutex_diccionario_recursos);

    sem_destroy(&sem_peticiones_io_por_procesar);
    sem_destroy(&sem_interfaz_io_libre);

    sem_destroy(&sem_stop_largo_plazo);
    sem_destroy(&sem_stop_corto_plazo);
    sem_destroy(&sem_stop_cpu_dispatch);
    sem_destroy(&sem_stop_io);

    sem_destroy(&mutex_victimas_pendientes_io);
    sem_destroy(&mutex_io_pendientes_ejecutando);
}

void init_colas(void){
    cola_new = queue_create();
    cola_ready  = queue_create();
    if(algoritmo_elegido == VRR) 
    cola_ready_aux  = queue_create();
    cola_execute  = queue_create();
    cola_blocked  = queue_create();
    cola_exit  = queue_create();
}

void liberar_colas(void){
    queue_destroy_and_destroy_elements(cola_new, (void*)liberar_elemento_pcb); 
    queue_destroy_and_destroy_elements(cola_ready, (void*)liberar_elemento_pcb);
    if (algoritmo_elegido == VRR) 
    queue_destroy_and_destroy_elements(cola_ready_aux, (void*)liberar_elemento_pcb);
    queue_destroy_and_destroy_elements(cola_execute, (void*)liberar_elemento_pcb);
    queue_destroy_and_destroy_elements(cola_blocked, (void*)liberar_elemento_pcb);
    queue_destroy_and_destroy_elements(cola_exit, (void*)free); // solo contendra punteros a pid     
}

void liberar_elemento_pcb(void* elemento){
    //printf("ELIMINADO ELEMENTO PCB\n");
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
        log_info(logger, "Se eligio el algoritmo de planificacion RR");
    }

    if(strcmp(config->algoritmo_planificacion,"VRR") == 0) {
        algoritmo_elegido = VRR;
        log_info(logger, "Se eligio el algoritmo de planificacion VRR");
    }
}

void init_diccionarios(void){
    iniciar_recursos();
    iniciar_interfaces(); // registro de todas la interfaces conectadas con el kernel

}

void liberar_diccionarios(void){
    liberar_recursos();
    liberar_interfaces(); 
}

void iniciar_recursos(void){
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
        existe_recursos = true;

	}
	else{
		log_info(logger, "No se Tiene Recursos \n");
        existe_recursos = false;
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

void iniciar_interfaces(void){
    interfaces = dictionary_create();
}

void liberar_interfaces(void){
    dictionary_destroy_and_destroy_elements(interfaces, liberar_elemento_interfaz);
}

void liberar_elemento_interfaz(void* elemento){
    t_interfaz* tmp = (t_interfaz*) elemento;

    sem_destroy(&tmp->semaforo);
    queue_destroy_and_destroy_elements(tmp->cola, (void*)liberar_elemento_t_io_pendiente);
    
    free(tmp);
}

void init_planificadores(void){
    pthread_create(&hilo_planificador_LP, NULL, (void*) func_largo_plazo, NULL);
    pthread_detach(hilo_planificador_LP);

    pthread_create(&hilo_planificador_CP, NULL, (void*) func_corto_plazo, NULL);
    pthread_detach(hilo_planificador_CP);
}

void init_listas(void){
    io_pendientes_ejecutando = list_create();
    victimas_pendientes_io = list_create();
    recursos_asignados = list_create();
}

void liberar_listas(void){
    list_destroy_and_destroy_elements(io_pendientes_ejecutando, liberar_elemento_t_io_pendiente);
    list_destroy_and_destroy_elements(victimas_pendientes_io, free);
    list_destroy_and_destroy_elements(recursos_asignados, liberar_elemento_t_registro_recurso);
}

void liberar_elemento_t_registro_recurso(void* elemento){
    t_registro_recurso* tmp = (t_registro_recurso*) elemento;
    free(tmp->recurso);
    free(tmp);
}

void init_manejador_de_procesos_io(void){
    pthread_t hilo_manejador_io;
    pthread_create(&hilo_manejador_io, NULL, (void*) manejador_de_procesos_pendientes_io, NULL);
    pthread_detach(hilo_manejador_io);
}