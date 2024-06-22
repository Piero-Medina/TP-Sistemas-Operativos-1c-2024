#include <io_pendiente/io_victima.h>


t_io_victima* crear_t_io_victima(uint32_t pid, tipo_victima motivo, char* nombre_interfaz) {
    t_io_victima* nueva_victima = malloc(sizeof(t_io_victima));
    
    if (nueva_victima == NULL) {
        perror("Error al asignar memoria");
        return NULL;
    }

    nueva_victima->pid = pid;
    nueva_victima->motivo_victima = motivo;

    if(nombre_interfaz == NULL){
        nueva_victima->nombre_interfaz = NULL;
    }else{
        nueva_victima->nombre_interfaz = strdup(nombre_interfaz);
    }

    return nueva_victima;
}


int posicion_de_io_victima_por_pid(uint32_t pid, t_list* lista) {
    t_io_victima* victima = NULL;
    int tamanio = list_size(lista);

    for (int i = 0; i < tamanio; i++) {
        victima = (t_io_victima*) list_get(lista, i);
        if (victima->pid == pid) {
            return i;
        }
    }

    return -1;
}

t_io_victima* buscar_io_victima_por_pid_y_obtener(uint32_t pid, t_list* lista) {
    int posicion = posicion_de_io_victima_por_pid(pid, lista);
    if (posicion != -1) {
        return (t_io_victima*) list_get(lista, posicion);
    } else {
        return NULL;
    }
}

t_io_victima* buscar_io_victima_por_pid_y_remover(uint32_t pid, t_list* lista) {
    int posicion = posicion_de_io_victima_por_pid(pid, lista);
    if (posicion != -1) {
        return (t_io_victima*) list_remove(lista, posicion);
    } else {
        return NULL;
    }
}

void liberar_elemento_t_io_victima(void* elemento) {
    t_io_victima* victima = (t_io_victima*) elemento;

    if (victima->nombre_interfaz != NULL) {
        free(victima->nombre_interfaz);
    }

    free(victima);
}