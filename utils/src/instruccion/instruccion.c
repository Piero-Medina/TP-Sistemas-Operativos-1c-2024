#include <instruccion/instruccion.h>

t_identificador convertir_a_identificador(char* identificador){
    t_identificador cod_identificador;

    if (strcmp(identificador, "SET") == 0) {
        cod_identificador = SET;
    }
    else if (strcmp(identificador, "SUM") == 0) {
        cod_identificador = SUM;
    }
    else if (strcmp(identificador, "SUB") == 0) {
        cod_identificador = SUB;
    }
    else if (strcmp(identificador, "MOV_IN") == 0) {
        cod_identificador = MOV_IN;
    }
    else if (strcmp(identificador, "MOV_OUT") == 0) {
        cod_identificador = MOV_OUT;
    }
    else if (strcmp(identificador, "RESIZE") == 0) {
        cod_identificador = RESIZE;
    }
    else if (strcmp(identificador, "JNZ") == 0) {
        cod_identificador = JNZ;
    }
    else if (strcmp(identificador, "COPY_STRING") == 0) {
        cod_identificador = COPY_STRING;
    }
    else if (strcmp(identificador, "IO_GEN_SLEEP") == 0) {
        cod_identificador = IO_GEN_SLEEP;
    }
    else if (strcmp(identificador, "IO_STDIN_READ") == 0) {
        cod_identificador = IO_STDIN_READ;
    }
    else if (strcmp(identificador, "IO_STDOUT_WRITE") == 0) {
        cod_identificador = IO_STDOUT_WRITE;
    }
    else if (strcmp(identificador, "IO_FS_CREATE") == 0) {
        cod_identificador = IO_FS_CREATE;
    }
    else if (strcmp(identificador, "IO_FS_DELETE") == 0) {
        cod_identificador = IO_FS_DELETE;
    }
    else if (strcmp(identificador, "IO_FS_TRUNCATE") == 0) {
        cod_identificador = IO_FS_TRUNCATE;
    }
    else if (strcmp(identificador, "IO_FS_WRITE") == 0) {
        cod_identificador = IO_FS_WRITE;
    }
    else if (strcmp(identificador, "IO_FS_READ") == 0) {
        cod_identificador = IO_FS_READ;
    }
    else if (strcmp(identificador, "WAIT") == 0) {
        cod_identificador = WAIT;
    }
    else if (strcmp(identificador, "SIGNAL") == 0) {
        cod_identificador = SIGNAL;
    }
    else if (strcmp(identificador, "EXIT") == 0) {
        cod_identificador = EXIT_I;
    }
    return cod_identificador;
}

char* convertir_a_string(t_identificador identificador){
    switch (identificador) {
        case SET:
            return "SET";
        case SUM:
            return "SUM";
        case SUB:
            return "SUB";
        case MOV_IN:
            return "MOV_IN";
        case MOV_OUT:
            return "MOV_OUT";
        case RESIZE:
            return "RESIZE";
        case JNZ:
            return "JNZ";
        case COPY_STRING:
            return "COPY_STRING";
        case IO_GEN_SLEEP:
            return "IO_GEN_SLEEP";
        case IO_STDIN_READ:
            return "IO_STDIN_READ";
        case IO_STDOUT_WRITE:
            return "IO_STDOUT_WRITE";
        case IO_FS_CREATE:
            return "IO_FS_CREATE";
        case IO_FS_DELETE:
            return "IO_FS_DELETE";
        case IO_FS_TRUNCATE:
            return "IO_FS_TRUNCATE";
        case IO_FS_WRITE:
            return "IO_FS_WRITE";
        case IO_FS_READ:
            return "IO_FS_READ";
        case WAIT:
            return "WAIT";
        case SIGNAL:
            return "SIGNAL";
        case EXIT_I:
            return "EXIT";
        default:
            return "UNKNOWN"; // Handle the case if the identifier is not recognized
    }
}


t_instruccion* crear_instruccion_mediante_array_string(char** split){
	t_instruccion* tmp = malloc(sizeof(t_instruccion));

	tmp->identificador = convertir_a_identificador(split[0]);
	tmp->parametros = list_create();

	int tamanio = string_array_size(split);

	if(tamanio > 1){
		int i = 1;
		while(split[i] != NULL){
			list_add(tmp->parametros, (void*) string_duplicate(split[i]));
			i++;
		}
	}

	return tmp;
}

void imprimir_instruccion(void* instruccion_ptr) {
    t_instruccion* instruccion = (t_instruccion*)instruccion_ptr;
    printf("Identificador: %d\n", instruccion->identificador);
    printf("Parámetros:\n");
    list_iterate(instruccion->parametros, imprimir_parametros);
}

void imprimir_parametros(void* parametro) {
    char* parametro_str = (char*)parametro;
    printf("  %s\n", parametro_str);
}

void destruir_list_instrucciones(t_list* instrucciones){
	list_destroy_and_destroy_elements(instrucciones, destruir_elemento_instruccion);
}

void destruir_elemento_instruccion(void* elemento) {
    t_instruccion* instruccion = (t_instruccion*)elemento;
    list_destroy_and_destroy_elements(instruccion->parametros,free);
    free(instruccion);
}