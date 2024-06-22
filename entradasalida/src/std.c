#include "std.h"

void leer_de_teclado(uint32_t pid, uint32_t bytes, t_list* direcciones_fisicas){
	char* leido = NULL;

    log_info(logger, "Ingrese un Valor de exactamente (%u) bytes", bytes);
	leido = readline("> ");

    while(strlen(leido) != (size_t) bytes){
        log_info(logger, "Ingrese un Valor de exactamente (%u) bytes", bytes);
		free(leido);
		leido = NULL;
		leido = readline("> ");
    }

    log_info(logger, "Se ingreso (%s)", leido);

    gestionar_escritura_multipagina(conexion_memoria, direcciones_fisicas, pid, (void*) leido, bytes, logger);

    free(leido);
}

void imprimir_por_pantalla(uint32_t pid, uint32_t bytes, t_list* direcciones_fisicas){
    void* data_leida = NULL;

    gestionar_lectura_multipagina(conexion_memoria, direcciones_fisicas, pid, data_leida, bytes, logger);

    char* string_leido = convertir_a_cadena_nueva(data_leida, (size_t) bytes);
    log_info(logger, "Valor Leido de Memoria (%s)", string_leido);
    free(string_leido);

    // alternativa 
    // write(1, data_leida, (size_t)bytes);

    free(data_leida);
}