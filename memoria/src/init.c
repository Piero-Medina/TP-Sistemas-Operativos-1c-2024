#include "init.h"

sem_t mutex_lista_de_procesos;
sem_t mutex_memoria_real;

bool procesar_conexion_en_ejecucion;

t_parser tabla[19];

t_list* lista_de_procesos;

void* memoria_real;

t_bitmap* bitmap;

void init_memoria(void){
    signal(SIGINT, sigint_handler);
    procesar_conexion_en_ejecucion = true;

    init_estructura_para_parsear();
    init_lista_de_procesos();
    init_semaforos();
    init_memoria_real();
    init_bitmap();
}

void sigint_handler(int signum){
    // printf("\n Finalizando el servidor por señal... \n");
    
    // finalizamos la ejecucion de cualquier hilo (que usa procesar_conexion_en_ejecucion) para que libere recursos correctamente
    procesar_conexion_en_ejecucion = false; 
    
    log_info(logger, "Finalizando MEMORIA \n\n\n");
    //libera_espacio_memoria(memoria,logger);
    liberar_memoria();
    exit(EXIT_SUCCESS);
}

void liberar_memoria(void){
    log_destroy(logger);
    liberar_memoria_config(config);
    liberar_conexion(server_fd);

    liberar_lista_de_procesos();
    liberar_semaforos();
    liberar_memoria_real();
    liberar_bitmap();
}

void init_semaforos(void){
    sem_init(&mutex_lista_de_procesos, 0, 1);
    sem_init(&mutex_memoria_real, 0, 1);
}

void liberar_semaforos(void){
    sem_destroy(&mutex_lista_de_procesos);
    sem_destroy(&mutex_memoria_real);
}

void init_estructura_para_parsear(void){
    tabla[0] = (t_parser){"SET", 2};
    tabla[1] = (t_parser){"SUM", 2};
    tabla[2] = (t_parser){"SUB", 2};
    tabla[3] = (t_parser){"MOV_IN", 2};
    tabla[4] = (t_parser){"MOV_OUT", 2};
    tabla[5] = (t_parser){"RESIZE", 1};
    tabla[6] = (t_parser){"JNZ", 2};
    tabla[7] = (t_parser){"COPY_STRING", 1};
    tabla[8] = (t_parser){"IO_GEN_SLEEP", 2};
    tabla[9] = (t_parser){"IO_STDIN_READ", 3};
    tabla[10] = (t_parser){"IO_STDOUT_WRITE", 3};
    tabla[11] = (t_parser){"IO_FS_CREATE", 2};
    tabla[12] = (t_parser){"IO_FS_DELETE", 2};
    tabla[13] = (t_parser){"IO_FS_TRUNCATE", 3};
    tabla[14] = (t_parser){"IO_FS_WRITE", 5};
    tabla[15] = (t_parser){"IO_FS_READ", 5};
    tabla[16] = (t_parser){"WAIT", 1};
    tabla[17] = (t_parser){"SIGNAL", 1};
    tabla[18] = (t_parser){"EXIT", 0};
}

void init_lista_de_procesos(void){
    lista_de_procesos = list_create();
}

void liberar_lista_de_procesos(void){
    list_destroy_and_destroy_elements(lista_de_procesos, liberar_elemento_proceso);
}

void liberar_elemento_proceso(void* elemento){
    t_proceso* proceso = (t_proceso*) elemento;
    destruir_list_instrucciones(proceso->instrucciones);

    if(proceso->tabla_paginas != NULL){
        liberar_lista_de_paginas(proceso->tabla_paginas);
    }

    free(proceso);
}

void init_memoria_real(void){
    log_info(logger, "Memoria Real disponible (%d) bytes", config->tam_memoria);
    memoria_real = malloc(config->tam_memoria);
}

void liberar_memoria_real(void){
    free(memoria_real);
}

void init_bitmap(void){
    int cantidad_frames = config->tam_memoria / config->tam_pagina;

    log_info(logger, "Tamanio de Pagina (%d) bytes", config->tam_pagina);
    log_info(logger, "Cantidad de Frames disponibles (%d) \n", cantidad_frames);

    bitmap = bitmap_crear(cantidad_frames);
}

void liberar_bitmap(void){
    bitmap_mostrar_detalles(bitmap);
    bitmap_liberar(bitmap);
}

