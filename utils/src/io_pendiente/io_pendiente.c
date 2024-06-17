#include <io_pendiente/io_pendiente.h>


t_io_pendiente* inicializar_io_pendiente(uint32_t pid, int operacion, bool interfaz_ocupada, char* param_string, t_list* peticiones, uint32_t param_int_1, uint32_t param_int_2, uint32_t param_int_3, uint32_t param_int_4){
    t_io_pendiente *nueva_io_pendiente = malloc(sizeof(t_io_pendiente));
    
    nueva_io_pendiente->pid = pid;
    nueva_io_pendiente->operacion = operacion;
    nueva_io_pendiente->interfaz_ocupada = interfaz_ocupada;
    
    if (param_string == NULL) {
        nueva_io_pendiente->parametro_string = NULL;
    } else {
        nueva_io_pendiente->parametro_string = strdup(param_string); // copia dinamica que luego se tiene que liberar
    }

    if (peticiones == NULL) {
        nueva_io_pendiente->peticiones_memoria = NULL;
    } else {
        nueva_io_pendiente->peticiones_memoria = peticiones;
    }

    nueva_io_pendiente->parametro_int_1 = param_int_1;
    nueva_io_pendiente->parametro_int_2 = param_int_2;
    nueva_io_pendiente->parametro_int_3 = param_int_3;
    nueva_io_pendiente->parametro_int_4 = param_int_4;
    
    return nueva_io_pendiente;
}

void imprimir_io_pendiente(t_io_pendiente* io){
    if (io == NULL) {
        fprintf(stderr, "Puntero nulo.\n");
        return;
    }

    printf("PID: %u\n", io->pid);
    printf("Operacion: %d\n", io->operacion);
    printf("Interfaz Ocupada: %s\n", io->interfaz_ocupada ? "true" : "false");
    printf("Parametro String: %s\n", io->parametro_string ? io->parametro_string : "NULL");
    printf("Parametro Int 1: %u\n", io->parametro_int_1);
    printf("Parametro Int 2: %u\n", io->parametro_int_2);
    printf("Parametro Int 3: %u\n", io->parametro_int_3);
    printf("Parametro Int 4: %u\n", io->parametro_int_4);

    printf("Peticiones Memoria:\n");
    int tamanio = list_size(io->peticiones_memoria);
    for (int i = 0; i < tamanio; i++) {
        t_peticion_memoria* peticion = (t_peticion_memoria*)list_get(io->peticiones_memoria, i);
        printf("Peticion %d:\n", i + 1);
        imprimir_peticion_memoria(peticion);
    }
}

t_io_pendiente* buscar_io_pendiente_por_pid_y_obtener(uint32_t pid, t_list* lista){
    int posicion = posicion_de_io_pendiente_por_pid(pid, lista);
    if(posicion != -1){
        return (t_io_pendiente*) list_get(lista, posicion);
    }else{
        return NULL;
    }
}

t_io_pendiente* buscar_io_pendiente_por_pid_y_remover(uint32_t pid, t_list* lista){
    int posicion = posicion_de_io_pendiente_por_pid(pid, lista);
    if(posicion != -1){
        return (t_io_pendiente*) list_remove(lista, posicion);
    }else{
        return NULL;
    }
}

int posicion_de_io_pendiente_por_pid(uint32_t pid, t_list* lista){
    t_io_pendiente* pendiente = NULL;
    int tamanio = list_size(lista);

    for (int i = 0; i < tamanio; i++){
        pendiente = (t_io_pendiente*) list_get(lista, i);
        if(pendiente->pid == pid){
            return i;
        }
    }

    return -1;
}

void liberar_elemento_t_io_pendiente(void* elemento){
    t_io_pendiente* tmp = (t_io_pendiente*) elemento;

    if(tmp->parametro_string != NULL){
        free(tmp->parametro_string);
    }

    if(tmp->peticiones_memoria != NULL){
        list_destroy_and_destroy_elements(tmp->peticiones_memoria, free); // (t_peticion_memoria*)
    }

    free(tmp);
}