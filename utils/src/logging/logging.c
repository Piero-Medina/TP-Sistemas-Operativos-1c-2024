#include <logging/logging.h>


t_log* iniciar_logger(char* file, char* nombre_proceso, t_log_level nivel) {
	//t_log* nuevo_logger = log_create(file, nombre_proceso, true, LOG_LEVEL_INFO);
	t_log* nuevo_logger = log_create(file, nombre_proceso, true, nivel);

    if (nuevo_logger == NULL){
		exit(EXIT_FAILURE);
	}

	return nuevo_logger;
}

t_log* iniciar_logger_oculto(char* file, char* nombre_proceso, t_log_level nivel) {
	//t_log* nuevo_logger = log_create(file, nombre_proceso, false, LOG_LEVEL_INFO);
	t_log* nuevo_logger = log_create(file, nombre_proceso, false, nivel);

    if (nuevo_logger == NULL){
		exit(EXIT_FAILURE);
	}

	return nuevo_logger;
}

