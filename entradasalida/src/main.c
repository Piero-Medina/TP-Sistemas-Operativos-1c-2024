#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>

#include <logging/logging.h>
#include <commons/log.h>
#include "entradaSalida_config.h"
#include <conexiones/conexiones.h>
#include <serializacion/serializacion.h>
#include <enum/enum.h>

#include <unistd.h> // para dormir

t_log* logger;
t_entradaSalida_config* config;
int conexion_kernel;
int conexion_memoria;

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        printf("Error: Se esperan dos parámetros\n");
        printf("%s <nombre_interfaz> <archivo_configuracion>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *nombre_interfaz = argv[1];
    char *archivo_configuracion = argv[2];

    logger = iniciar_logger("entradaSalida.log", nombre_interfaz);
    log_info(logger, "Iniciando Modulo ENTRADA_SALIDA de nombre (%s) \n", nombre_interfaz);

    config = init_entradaSalida_config(archivo_configuracion);

    log_info(logger, "Tipo: %s \n", config->tipo_interfaz);

    conexion_kernel = crear_conexion(config->ip_kernel, config->puerto_kernel, "KERNEL", logger);
    enviar_handshake(conexion_kernel, HANDSHAKE, nombre_interfaz, "KERNEL", logger);

    conexion_memoria = crear_conexion(config->ip_memoria, config->puerto_memoria, "MEMORIA", logger);
    enviar_handshake(conexion_memoria, HANDSHAKE, nombre_interfaz, "MEMORIA", logger);

    // dormimos
    sleep(5);

    log_destroy(logger);
    liberar_entradaSalida_config(config);
    liberar_conexion(conexion_kernel);
    liberar_conexion(conexion_memoria);

    return 0;
}
