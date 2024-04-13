#include "init.h"

bool procesar_conexion_en_ejecucion;

void init_kernel(void){
    signal(SIGINT, sigint_handler);
    procesar_conexion_en_ejecucion = true;
}

void sigint_handler(int signum){
    printf("\n Finalizando el servidor por señal... \n");
    
    // finalizamos la ejecucion de cualquier hilo (que usa procesar_conexion_kernel) para que libere recursos correctamente
    procesar_conexion_en_ejecucion = false; 
    
    liberar_kernel();

    exit(EXIT_SUCCESS);
}

void liberar_kernel(void){
    log_destroy(logger);
    liberar_kernel_config(config);
    liberar_conexion(conexion_cpu_dispatch);
    liberar_conexion(conexion_cpu_interrupt);
    liberar_conexion(conexion_memoria);
    liberar_conexion(server_fd);
}
