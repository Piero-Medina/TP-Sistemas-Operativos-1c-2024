#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>

#include <logging/logging.h>
#include <commons/log.h>
#include "entradaSalida_config.h"
#include <conexiones/conexiones.h>
#include <serializacion/serializacion.h>
#include <enum/enum.h>

#include "generales.h"
#include <comunicacion/comunicacion.h>
#include "init.h"

#include <stdint.h>
#include <unistd.h> // para dormir
#include "bloques_dialFs.h"
#include "bit_map_dialFs.h"

t_log* logger;
t_entradaSalida_config* config;

int conexion_kernel;
int conexion_memoria;


int main(int argc, char* argv[]) {
    if(argc != 3){
        printf("Error: Se esperan dos parámetros\n");
        printf("%s <nombre_interfaz> <archivo_configuracion>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *nombre_interfaz = argv[1];
    char *archivo_configuracion = argv[2];

    logger = iniciar_logger("entradaSalida.log", nombre_interfaz, LOG_LEVEL_INFO);
    log_info(logger, "Iniciando Modulo ENTRADA_SALIDA de nombre (%s) \n", nombre_interfaz);

    tipo_de_interfaz_elegido(archivo_configuracion);

    config = init_entradaSalida_config(archivo_configuracion, tipo_de_interfaz);

    init_entrada_salida();
    
    conexion_kernel = crear_conexion(config->ip_kernel, config->puerto_kernel, "KERNEL", logger);
    enviar_handshake(conexion_kernel, HANDSHAKE, nombre_interfaz, "KERNEL", logger);

    if(!(tipo_de_interfaz == GENERICA)){
        conexion_memoria = crear_conexion(config->ip_memoria, config->puerto_memoria, "MEMORIA", logger);
        enviar_handshake(conexion_memoria, HANDSHAKE, nombre_interfaz, "MEMORIA", logger);
    }

    log_info(logger, "Registrando Interfaz ante el Kernel");
    envio_generico_entero_y_string(conexion_kernel, REGISTRO_INTERFAZ, tipo_de_interfaz, nombre_interfaz);
    validar_respuesta_op_code(conexion_kernel, KERNEL_OK, logger);

    configurar_segun_tipo_de_interfaz();

    liberar_entrada_salida();

    return 0;
}
