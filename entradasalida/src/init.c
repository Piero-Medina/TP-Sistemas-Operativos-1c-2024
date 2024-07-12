#include "init.h"

tipo_interfaz tipo_de_interfaz;
bool procesar_conexion_en_ejecucion;

t_bitmap* bitmap;

char* path_bitmap;
char* path_bloques;
char* path_metadatos;
char* path_archivos;

t_dictionary* diccionario_de_archivos;

sem_t sem_compactacion;

void init_entrada_salida(void){
    signal(SIGINT, sigint_handler);
    procesar_conexion_en_ejecucion = true;

    init_semaforos();
}

void liberar_entrada_salida(void){
    log_destroy(logger);
    liberar_entradaSalida_config(config);
    liberar_conexion(conexion_kernel);
    liberar_conexion(conexion_memoria);

    liberar_semaforos();
    
    if(tipo_de_interfaz == DIALFS){
        bitmap_liberar(bitmap);
        free(path_bloques);
        free(path_bitmap);
        free(path_metadatos);
        free(path_archivos);
        dictionary_destroy_and_destroy_elements(diccionario_de_archivos, liberar_elemento_t_archivo);
    }
}

void sigint_handler(int signum){
    // printf("\n Finalizando el servidor por señal... \n");
    procesar_conexion_en_ejecucion = false;
    log_info(logger, "Finalizando ENTRADASALIDA \n\n\n"); 
    liberar_entrada_salida();

    exit(EXIT_SUCCESS);
}

void tipo_de_interfaz_elegido(char* path){
    t_config* config_tmp = config_create(path);
    char* tipo_interfaz = config_get_string_value(config_tmp, "TIPO_INTERFAZ");

    if(strcmp(tipo_interfaz, "GENERICA") == 0) {
        tipo_de_interfaz = GENERICA;
        log_info(logger, "Tipo De Interfaz: %s \n", tipo_interfaz);
    }

    if(strcmp(tipo_interfaz,"STDIN") == 0) {
        tipo_de_interfaz = STDIN;
        log_info(logger, "Tipo De Interfaz: %s \n", tipo_interfaz);
    }

    if(strcmp(tipo_interfaz,"STDOUT") == 0) {
        tipo_de_interfaz = STDOUT;
        log_info(logger, "Tipo De Interfaz: %s \n", tipo_interfaz);
    }

    if(strcmp(tipo_interfaz,"DIALFS") == 0) {
        tipo_de_interfaz = DIALFS;
        log_info(logger, "Tipo De Interfaz: %s \n", tipo_interfaz);
    }

    config_destroy(config_tmp);
}


void configurar_segun_tipo_de_interfaz(void){
    pthread_t hilo;

    if(tipo_de_interfaz == GENERICA){
        log_info(logger, "Configurando interfaz en modo GENERICA");

        pthread_create(&hilo, NULL, (void*) procesar_conexion_siendo_io_generica, NULL);
        pthread_join(hilo, NULL);
    }

    if(tipo_de_interfaz == STDIN){
        log_info(logger, "Configurando interfaz en modo STDIN");

        pthread_create(&hilo, NULL, (void*) procesar_conexion_siendo_io_stdin, NULL);
        pthread_join(hilo, NULL);
    }

    if(tipo_de_interfaz == STDOUT){
        log_info(logger, "Configurando interfaz en modo GENERICA");

        pthread_create(&hilo, NULL, (void*) procesar_conexion_siendo_io_stdout, NULL);
        pthread_join(hilo, NULL);
    }

    if(tipo_de_interfaz == DIALFS){
        log_info(logger, "Configurando interfaz en modo DIALFS");

        init_file_system();
        
        pthread_create(&hilo, NULL, (void*) procesar_conexion_siendo_io_dialFs, NULL);
        pthread_join(hilo, NULL);
    }
}

void comprobar_directorio(char* path){
    int resultado = verificar_directorio(path);
    if (resultado == 0) {
        log_info(logger, "Directorio (%s) existe", path);
    } else if (resultado == 1) {
        log_info(logger, "Directorio (%s) creado", path);
    } else {
        log_error(logger, "Ha ocurrido un error al intentar crear el directorio(%s)", path);
        exit(EXIT_FAILURE);
    }
}

void crear_bitmap(char* path){
    bitmap = bitmap_crear(path, config->block_count);
    if (bitmap == NULL){
        log_error(logger,"Error al crear Archivo bitmap (%s)", path);
        exit(EXIT_FAILURE);
    }
    log_info(logger, "Archivo bitmap (%s) levantado", path);
}

void init_file_system(void){
    path_bloques = concatenar_ruta(config->path_base_dialfs, "bloques.dat");
    path_bitmap = concatenar_ruta(config->path_base_dialfs, "bitmap.dat");
    path_metadatos = concatenar_ruta(config->path_base_dialfs, "metadatos");
    path_archivos = concatenar_ruta(config->path_base_dialfs, "archivos");
        
    comprobar_directorio(config->path_base_dialfs);
    crear_archivo_de_bloques(path_bloques);
    crear_bitmap(path_bitmap);
    comprobar_directorio(path_metadatos);
    comprobar_directorio(path_archivos);

    diccionario_de_archivos = dictionary_create();
    levantar_diccionario_de_archivos(diccionario_de_archivos);
}

void init_semaforos(void){
    sem_init(&sem_compactacion, 0, 1);
}

void liberar_semaforos(void){
    sem_destroy(&sem_compactacion);
}