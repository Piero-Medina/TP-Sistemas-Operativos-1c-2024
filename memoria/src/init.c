#include "init.h"

bool procesar_conexion_en_ejecucion;

void init_memoria(void){
    signal(SIGINT, sigint_handler);
    procesar_conexion_en_ejecucion = true;
}

void sigint_handler(int signum){
    printf("\n Finalizando el servidor por señal... \n");
    
    // finalizamos la ejecucion de cualquier hilo (que usa procesar_conexion_en_ejecucion) para que libere recursos correctamente
    procesar_conexion_en_ejecucion = false; 
    
    liberar_memoria();

    exit(EXIT_SUCCESS);
}

void liberar_memoria(void){
    log_destroy(logger);
    liberar_memoria_config(config);
    liberar_conexion(server_fd);
}