#include "modelo_de_memoria.h"

//defino mi memoria de trabajo para esta ejecucion del S.O.
t_memoria_paginada* inicializar_memoria_paginada(int tamano, int tamano_pagina, t_log* logger){
    /*Defino la cantidad de paginas*/
    unsigned int cantidad_paginas = tamano/tamano_pagina;
    t_memoria_paginada* memoria = (t_memoria_paginada*) malloc(sizeof(t_memoria_paginada));
    
    /*Pido la memoria para los elementos*/
    memoria->tamano_memoria = tamano;
    memoria->tamano_pagina = tamano_pagina;
    memoria->paginas = (t_pagina* ) malloc (sizeof(t_pagina) * cantidad_paginas);
   if(memoria->paginas == NULL){
        log_error(logger,"No se pudo asignar memoria a la memoria paginada");
        return NULL;
    }

    /*asigno la memoria*/
    for(int i = 0; i < cantidad_paginas; i++){
        memoria->paginas[i].marco = i;/*Defino el marco como el i actual*/
        memoria->paginas[i].dato = (char*) malloc (sizeof(char* ) * tamano_pagina);
        memoria->paginas[i].asignada = false;
        if(memoria->paginas[i].dato == NULL){
            log_error(logger,"No se pudo asignar memoria a la pagina");
            return NULL;
        }
    }
    return memoria;
}
/*liberar memoria*/
void libera_espacio_memoria(t_memoria_paginada* memoria,t_log* logger){
    int cantidad_paginas = memoria->tamano_memoria/memoria->tamano_pagina;
    //libero todas las paginas
    for(int i = 0; i < cantidad_paginas; i++){
        free(memoria->paginas[i].dato);
    }
    //libero la memoria
    free(memoria->paginas);
    free(memoria);
}

/*Lectura de Memoria */

bool escribir_memoria(t_memoria_paginada* memoria,int marco, int offset,t_log* logger){

    return true;
}
bool leer_memoria(t_memoria_paginada* memoria,int marco, int offset,t_log* logger){

    return true;
}
/*Gestion de procesos*/
bool resizear_memoria_de_proceso(t_proceso* proceso, t_memoria_paginada* memoria, int valor_resize ,t_log* logger){
    int marcos_asignados = list_size(proceso->marcos);
    int marcos_a_asignar = valor_resize/memoria->tamano_pagina;
    int paginas_a_agregar = marcos_a_asignar - marcos_asignados;
    if(paginas_a_agregar > 0){
        for(int i = 0; i < paginas_a_agregar; i++){
            if(asignar_paginas_a_proceso(proceso, memoria,logger)==false){
                log_info(logger,"No se pudo asignar la memoria");
                return false;
            }
            log_info(logger,"nueva pagina de memoria asignada");
        }
    }
    if(paginas_a_agregar< 0){
        for(int i = 0; i < paginas_a_agregar; i++){
            if(liberar_paginas_de_proceso(proceso, memoria,proceso->marcos->elements_count -1, logger)==false){
                log_info(logger,"No se pudo asignar la memoria");
                return false;
            }
            log_info(logger,"nueva pagina de memoria asignada");
        }
    }
    return true;
}

bool asignar_paginas_a_proceso(t_proceso* proceso,t_memoria_paginada* memoria,t_log* logger){
    t_pagina* mem_libre = esta_libre(memoria,logger);
    if(mem_libre == NULL){
        log_error(logger,"no se encontro memoria disponible para asignar");
        return false;
    }
    mem_libre->asignada = true;
    list_add(proceso->marcos,mem_libre->marco);
    return true;
}

bool liberar_paginas_de_proceso(t_proceso* proceso,t_memoria_paginada* memoria,int marco, t_log* logger){
    if(memoria->paginas[marco].asignada == false){
        log_error(logger,"no se encuentra asignada la memoria pedida");
        return false;
    }
    bool destroy_condition(void* data){
    t_pagina* pagina = (t_pagina*) data;
    return pagina->marco == marco;
    }
    /*liberar int*/
    void destroy_int(void* data) {
        free(data);
    }
    memoria->paginas[marco].asignada = false;
    list_remove_and_destroy_by_condition(proceso->marcos,destroy_condition,destroy_int);
    return true;
}


t_pagina* esta_libre(t_memoria_paginada* memoria,t_log* logger){
    int total_paginas = memoria->tamano_memoria/memoria->tamano_pagina;
    for(int i = 0; i < total_paginas; i++){
        if(memoria->paginas[i].asignada == false){
            return &(memoria->paginas[i]);
            log_info(logger, "Se selecciono una pagina de memoria");
        }
    }
    log_error(logger, "no hay memoria disponible para asignar");

    return NULL;
}

