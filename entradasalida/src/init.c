#include "init.h"

tipo_interfaz tipo_de_interfaz;
bool procesar_conexion_en_ejecucion;


void init_entrada_salida(void){
    signal(SIGINT, sigint_handler);
    procesar_conexion_en_ejecucion = true;
}

void liberar_entrada_salida(void){
    log_destroy(logger);
    liberar_entradaSalida_config(config);
    liberar_conexion(conexion_kernel);
    liberar_conexion(conexion_memoria);
}

void sigint_handler(int signum){
    printf("\n Finalizando el servidor por señal... \n");
    procesar_conexion_en_ejecucion = false;
    log_info(logger, "¡Este proceso nunca debió existir! \n\n\n"); 
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
        pthread_create(&hilo, NULL, (void*) procesar_conexion_siendo_io_generica, NULL);
        pthread_join(hilo, NULL);
    }
}