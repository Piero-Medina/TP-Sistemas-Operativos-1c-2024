#include "peticiones_kernel.h"

/////////////////////////////////////////////////////////////////////////////
void kernel_creacion_nuevo_proceso(int conexion){
    int pid;
    char* path_intrucciones;
    t_proceso* proceso = NULL;

    recibir_generico_entero_string(conexion, &pid, &path_intrucciones);

    proceso = crear_proceso_mediante_path(pid, path_intrucciones);

    list_add(lista_de_procesos, proceso);

    free(path_intrucciones);
}

t_proceso* crear_proceso_mediante_path(int pid, char* path_intrucciones){
    
    int posicion = 0;
    char** split = NULL;
    t_instruccion* intruccion = NULL;
    t_list* lista_intrucciones = list_create();

    FILE* archivo = leer_archivo(path_intrucciones);
    char* buffer = malloc(sizeof(char) * MAX_READ);
    char* linea = NULL; // no es necesario liberar

    while (fgets(buffer, MAX_READ, archivo) != NULL){

        linea = strtok(buffer, "\n");
        split = string_split(linea, " ");

        if(instruccion_permitida(split, posicion)){
	    	intruccion = crear_instruccion_mediante_array_string(split);
	    	list_add(lista_intrucciones, intruccion);
	    }
        else{
            exit(EXIT_FAILURE);
        }

        free(buffer);
        buffer = malloc(sizeof(char) * MAX_READ);
        string_array_destroy(split);
        split = NULL;
    }

    free(buffer);
    cerrar_archivo(archivo);
    //string_array_destroy(split); no es necesario ya que queda en NULL si no entra al while

    t_proceso* proceso = malloc(sizeof(t_proceso));
    proceso->pid = pid;
    proceso->instrucciones = lista_intrucciones;

    return proceso;
}

bool instruccion_permitida(char** split, int posicion){
	if(existe_Token(split[0], &posicion)){
		if(cantidad_parametros_correctos(split, posicion)){
            return true;
		}
		else{
			printf("cant de parametros incorrectos del token -> %s \n",split[0]);
			return false;
		}
	}
	else{
		printf("no existe el Token -> %s \n",split[0]);
		return false;
	}
}

bool existe_Token(char* token, int* pos){
	for(int i = 0; i < 19; i++){
		if(strcmp(tabla[i].intruccion, token) == 0){
			*pos = i;
			return true;
		}
	}
	return false;
}

bool cantidad_parametros_correctos(char** split, int posicion){
	if(string_array_size(split) == (tabla[posicion].cant_parametros + 1)){
        //printf("token => %s \n", split[0]);
        //printf("largo split : \n", string_array_size(split));
        //printf("cant de parametros esperables => %d \n", tabla[posicion].cant_parametros);
		return true;
	}
	return false;
}

void imprimir_array_strings(char** array) {
    for (int i = 0; array[i] != NULL; i++) {
        printf("%s\n", array[i]);
    }
}

void imprimir_lista_procesos(t_list* lista_procesos) {
    printf("!LISTA DE PROCESOS¡\n");
    list_iterate(lista_procesos, imprimir_proceso);
}

void imprimir_proceso(void* proceso_ptr) {
    t_proceso* proceso = (t_proceso*)proceso_ptr;
    printf("!PROCESO¡\n");
    printf("pid: %d\n", proceso->pid);
    printf("intrucciones:\n");
    list_iterate(proceso->instrucciones, imprimir_instruccion);
}

/////////////////////////////////////////////////////////////////////////////
