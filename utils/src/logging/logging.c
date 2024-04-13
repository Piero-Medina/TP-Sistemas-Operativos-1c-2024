#include <logging/logging.h>


t_log* iniciar_logger(char* file, char* nombre_proceso) {
	t_log* nuevo_logger = log_create(file, nombre_proceso, false, LOG_LEVEL_INFO);

    if (nuevo_logger == NULL){
		exit(EXIT_FAILURE);
	}

	return nuevo_logger;
}