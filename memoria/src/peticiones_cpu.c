#include "peticiones_cpu.h"

t_instruccion* buscar_intruccion(int pid, int program_counter){
    t_proceso* proceso = buscar_proceso(pid);
    return list_get(proceso->instrucciones, program_counter);
}

t_proceso* buscar_proceso(int pid){
    bool coincide_con_pid(void* elemento){
        t_proceso* tmp = (t_proceso*) elemento;
        return tmp->pid == pid;
    }
    
    return (t_proceso*) list_find(lista_de_procesos, coincide_con_pid);
}

int32_t buscar_marco(int pid, int numero_pagina){
    t_proceso* proceso = buscar_proceso(pid);
    
    // si se trata de pedir un frame a un proceso que aun no tiene paginas
    if(proceso->tabla_paginas == NULL){
        return -1; 
    }

    t_pagina* pagina = list_get(proceso->tabla_paginas, numero_pagina);

    return (int32_t) pagina->numero_frame;
}

int resize_proceso(uint32_t pid, uint32_t tamanio){
    int cantidad_paginas_original, cantidad_paginas_nuevo;
    int diferencia, cant_paginas_libres;

    sem_wait(&mutex_lista_de_procesos);
        t_proceso* proceso = buscar_proceso((int) pid);
    sem_post(&mutex_lista_de_procesos);

    if (proceso->tamanio == tamanio){
        // No hacer nada
        return NO_PROCESAR;
    }

    if (proceso->tamanio == 0){
        crear_tabla_de_paginas(pid, tamanio);
        return TODO_OK;
    }

    diferencia = tamanio - proceso->tamanio;   
    cant_paginas_libres = bitmap_cantidad_de_bloques_libres(bitmap);

    // no hay paginas libres
    if(diferencia > (cant_paginas_libres * config->tam_pagina)){
        return OUT_OF_MEMORY;
    }

    cantidad_paginas_original = cantidad_de_bloques_ocupados(proceso->tamanio, config->tam_pagina);
    cantidad_paginas_nuevo = cantidad_de_bloques_ocupados(tamanio, config->tam_pagina);

    if(cantidad_paginas_original < cantidad_paginas_nuevo){
        log_info(logger, "PID: <%u> - Tamaño Actual: <%d> - Tamanio a Ampliar: <%d>", pid, proceso->tamanio, diferencia);
        ampliar_proceso(pid, tamanio);
    } 
    if(cantidad_paginas_original > cantidad_paginas_nuevo){
        log_info(logger, "PID: <%u> - Tamaño Actual: <%d> - Tamanio a reducir: <%d>", pid, proceso->tamanio, (diferencia * -1));
        reducir_proceso(pid, tamanio); 
    }

    return TODO_OK;
}

void ampliar_proceso(uint32_t  pid, uint32_t tamanio){
    int cantidad_paginas_original, cantidad_paginas_nuevo;
    int diferencia_paginas, nro_frame_libre;

    sem_wait(&mutex_lista_de_procesos);
        t_proceso* proceso = buscar_proceso((int) pid);
    sem_post(&mutex_lista_de_procesos);

    uint32_t numero_pag = proceso->tabla_paginas->elements_count - 1;

    cantidad_paginas_original = cantidad_de_bloques_ocupados(proceso->tamanio, config->tam_pagina);
    cantidad_paginas_nuevo = cantidad_de_bloques_ocupados(tamanio, config->tam_pagina);

    diferencia_paginas = cantidad_paginas_nuevo - cantidad_paginas_original;

    for(int i = 0; i < diferencia_paginas; i++){
        nro_frame_libre = bitmap_bloque_libre(bitmap);
        numero_pag += 1;

        t_pagina* pagina = crear_pagina(numero_pag, (uint32_t)nro_frame_libre);
        list_add(proceso->tabla_paginas, (void*) pagina);

        bitmap_marcar_bloque_como_ocupado(bitmap, nro_frame_libre);
    }

    proceso->tamanio = (int)tamanio;

}

void reducir_proceso(uint32_t pid, uint32_t tamanio){
    int cantidad_paginas_original, cantidad_paginas_nuevo;
    int diferencia_paginas, cantidad_paginas;

    if(tamanio == 0){
        vaciar_proceso(pid);
        return;
    }

    sem_wait(&mutex_lista_de_procesos);
        t_proceso* proceso = buscar_proceso((int) pid);
    sem_post(&mutex_lista_de_procesos);

    cantidad_paginas_original = cantidad_de_bloques_ocupados(proceso->tamanio, config->tam_pagina);
    cantidad_paginas_nuevo = cantidad_de_bloques_ocupados(tamanio, config->tam_pagina);

    diferencia_paginas = cantidad_paginas_original - cantidad_paginas_nuevo;

    // da el tamanio en base 1
    cantidad_paginas = proceso->tabla_paginas->elements_count;

    for (int i = 0; i < diferencia_paginas; i++){
        t_pagina* pagina = (t_pagina*) list_remove(proceso->tabla_paginas, (cantidad_paginas - 1) - i);

        bitmap_marcar_bloque_como_libre(bitmap, (int) pagina->numero_frame);
        liberar_elemento_pagina((void*) pagina);
    }

    proceso->tamanio = (int)tamanio;
}

void crear_tabla_de_paginas(uint32_t pid ,uint32_t tamanio){
    int cantidad_frames, nro_frame_libre;

    sem_wait(&mutex_lista_de_procesos);
        t_proceso* proceso = buscar_proceso((int) pid);
    sem_post(&mutex_lista_de_procesos);

    cantidad_frames = cantidad_de_bloques_ocupados(tamanio, config->tam_pagina);

    proceso->tabla_paginas = list_create();

    for (int i = 0; i < cantidad_frames; i++){
        nro_frame_libre = bitmap_bloque_libre(bitmap);

        t_pagina* pagina = crear_pagina(i, (uint32_t)nro_frame_libre);
        list_add(proceso->tabla_paginas, (void*) pagina);

        bitmap_marcar_bloque_como_ocupado(bitmap, nro_frame_libre);
    }

    proceso->tamanio = (int)tamanio;
}

void vaciar_proceso(uint32_t pid){
    int cantidad_paginas;

    sem_wait(&mutex_lista_de_procesos);
        t_proceso* proceso = buscar_proceso((int) pid);
    sem_post(&mutex_lista_de_procesos);

    // da el tamanio en base 1
    cantidad_paginas = proceso->tabla_paginas->elements_count;

    for (int i = 0; i < cantidad_paginas; i++){
        t_pagina* pagina = (t_pagina*) list_remove(proceso->tabla_paginas, (cantidad_paginas - 1) - i);

        bitmap_marcar_bloque_como_libre(bitmap, (int) pagina->numero_frame);
        liberar_elemento_pagina((void*) pagina);
    }

    list_destroy(proceso->tabla_paginas);

    proceso->tamanio = 0;
    proceso->tabla_paginas = NULL;

    log_info(logger, "PID: <%d> - Sin Paginas ", pid);
}