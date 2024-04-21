#include "consola_interactiva.h"

void consola_interactiva(void){

    char* leido = NULL;

    menu();
    leido = readline("> ");
    procesar_opcion(leido);

    while (!(strlen(leido) == 0)){
        free(leido);
        leido = NULL;
        menu();
        leido = readline("> ");
        procesar_opcion(leido);
    }
    
    free(leido);
}

void menu(void){

    printf("\nCamandos Disponibles: \n\n");

    printf("Ejecutar Script de Instrucciones: EJECUTAR_SCRIPT [PATH]\n");
    printf("Iniciar proceso: INICIAR_PROCESO [PATH]\n");
    printf("Finalizar proceso: FINALIZAR_PROCESO [PID]\n");
    printf("Detener planificación: DETENER_PLANIFICACION\n");
    printf("Iniciar planificación: INICIAR_PLANIFICACION\n");
    printf("Listar procesos por estado: PROCESO_ESTADO\n\n");

    printf("Ingrese el comando de la opción deseada: \n");
}

void procesar_opcion(char* leido){
    t_opcion_consola opcion = opcion_seleccionada(leido);
    ejecutar_opcion(opcion, leido);
    
}

t_opcion_consola opcion_seleccionada(char* leido){
    
    t_opcion_consola tmp;
    char** split = NULL;
    split = string_split(leido, " ");

    if(strcmp(split[0], "EJECUTAR_SCRIPT") == 0){
		tmp.opcion = EJECUTAR_SCRIPT;
        tmp.cant_parametros = 1;
	}
	else if(strcmp(split[0], "INICIAR_PROCESO") == 0){
		tmp.opcion = INICIAR_PROCESO;
        tmp.cant_parametros = 1;
	}
	else if(strcmp(split[0], "FINALIZAR_PROCESO") == 0){
        tmp.opcion = FINALIZAR_PROCESO;
        tmp.cant_parametros = 1;
	}
	else if(strcmp(split[0], "DETENER_PLANIFICACION") == 0){
		tmp.opcion = DETENER_PLANIFICACION;
        tmp.cant_parametros = 0;
	}
	else if(strcmp(split[0], "INICIAR_PLANIFICACION") == 0){
		tmp.opcion = INICIAR_PLANIFICACION;
        tmp.cant_parametros = 0;
	}
    else if(strcmp(split[0], "PROCESO_ESTADO") == 0){
		tmp.opcion = PROCESO_ESTADO;
        tmp.cant_parametros = 0;
	}

    string_array_destroy(split);
    return tmp;
}

void ejecutar_opcion(t_opcion_consola opcion, char* leido){
    switch(opcion.opcion) {
        case EJECUTAR_SCRIPT:
            printf("Se seleccionó la opción EJECUTAR_SCRIPT\n");
            func_ejecutar_script(leido);
            break;
        case INICIAR_PROCESO:
            printf("Se seleccionó la opción INICIAR_PROCESO\n");
            func_iniciar_proceso(leido);
            break;
        case FINALIZAR_PROCESO:
            printf("Se seleccionó la opción FINALIZAR_PROCESO\n");
            func_finalizar_proceso(leido);
            break;
        case DETENER_PLANIFICACION:
            printf("Se seleccionó la opción DETENER_PLANIFICACION\n");
            func_detener_planificacion();
            break;
        case INICIAR_PLANIFICACION:
            printf("Se seleccionó la opción INICIAR_PLANIFICACION\n");
            func_iniciar_planificacion();
            break;
        case PROCESO_ESTADO:
            printf("Se seleccionó la opción PROCESO_ESTADO\n");
            func_proceso_estado();
            break;
        default:
            printf("Opción no válida\n");
            break;
    }
}

void func_ejecutar_script(char* leido){
    printf("Ejecutando comando func_ejecutar_script\n");

    char** split = NULL;
    split = string_split(leido, " ");

    FILE* archivo = leer_archivo(split[1]);

    char* buffer = malloc(sizeof(char) * MAX);
    char* linea = NULL; // no es necesario liberar

    while (fgets(buffer, MAX, archivo) != NULL){

        linea = strtok(buffer, "\n");
        procesar_opcion(linea);

        free(buffer);
        buffer = malloc(sizeof(char) * MAX);
    }

    free(buffer);
    cerrar_archivo(archivo);
    string_array_destroy(split);
}

void func_iniciar_proceso(char* leido){
    printf("Ejecutando comando func_iniciar_proceso\n");
    char** split = string_split(leido, " ");

    sem_wait(&mutex_pid);
    t_PCB* pcb = crear_PCB(contador_pid, config->quantum, NEW); // creado pero aun no liberado
    contador_pid ++;
    sem_post(&mutex_pid);

    log_info(logger, "Avisando a Memoria entrada de Nuevo Preceso PID: <%d> ", pcb->pid);
    sem_wait(&mutex_conexion_memoria);
    avisar_nuevo_proceso_memoria(conexion_memoria, pcb->pid, split[1]);
    sem_post(&mutex_conexion_memoria);

    mover_a_new(pcb);    

    string_array_destroy(split);
}

// TODO
void func_finalizar_proceso(char* leido){
    printf("Ejecutando comando func_finalizar_proceso\n");
    char** split = string_split(leido, " ");

    // split[1] -> PID (proceso a finalizar, liberando recursos)

    string_array_destroy(split);
}

// TODO
void func_detener_planificacion(void){
    printf("Ejecutando comando func_detener_planificacion\n");
    // detener planificacion (t_sem)
}

// TODO
void func_iniciar_planificacion(void){
    printf("Ejecutando comando func_iniciar_planificacion\n");
    // iniciar planificacion (t_sem)
}

// TODO
void func_proceso_estado(void){
    printf("Ejecutando comando func_proceso_estado\n");

    char* lista_new = string_aplanar_PID(cola_new->elements);
    char* lista_ready = string_aplanar_PID(cola_ready->elements);
    char* lista_execute = string_aplanar_PID(cola_execute->elements);
    char* lista_blocked = string_aplanar_PID(cola_blocked->elements);
    char* lista_blocked_aux = NULL; // la declaraciones deben hacerse afuera del if
    if (algoritmo_elegido == VRR)
    lista_blocked_aux = string_aplanar_PID(cola_blocked_aux->elements);
    char* lista_exit = string_aplanar_PID(cola_exit->elements);

    printf("proceso en NEW : %s \n", lista_new);
    printf("proceso en READY : %s \n", lista_ready);
    printf("proceso en EXECUTE : %s \n", lista_execute);
    printf("proceso en BLOCKED : %s \n", lista_blocked);
    if (algoritmo_elegido == VRR) 
    printf("proceso en BLOCKED AUX : %s \n", lista_blocked_aux);
    printf("proceso en EXIT : %s \n", lista_exit);

    free(lista_new);
    free(lista_ready);
    free(lista_execute);
    free(lista_blocked);
    if (algoritmo_elegido == VRR) 
    free(lista_blocked_aux);
    free(lista_exit);
}

FILE* leer_archivo(char *path){
	FILE * archivo = fopen(path, "r");
	if(archivo == NULL){
		printf("no se pudo leer archivo");
		exit(EXIT_FAILURE);
	}
	return archivo;
}

void cerrar_archivo(FILE* archivo){
    fclose(archivo);
}
    