#include "planificacion.h"

void mover_a_new(t_PCB* pcb){
    sem_wait(&mutex_cola_new);
        queue_push(cola_new, (void*) pcb);
    sem_post(&mutex_cola_new);

    log_info(logger, "Se crea el proceso PID: <%u> en NEW", pcb->pid);
    
    sem_post(&sem_procesos_esperando_en_new);

}

void func_largo_plazo(void* arg){
    while (procesar_conexion_en_ejecucion){
        sem_wait(&sem_procesos_esperando_en_new);
        sem_wait(&sem_grado_multiprogramacion);

        if(sistema_detenido){
            stop_largo_plazo = true;
            log_info(logger, "STOP LARGO PLAZO");
            sem_wait(&sem_stop_largo_plazo);
            log_info(logger, "RESUME LARGO PLAZO");
        }

        mover_new_a_ready();
    }
}

void mover_new_a_ready(void){
    sem_wait(&mutex_cola_new);
        t_PCB* pcb = (t_PCB*) queue_pop(cola_new);
    sem_post(&mutex_cola_new);

    pcb->estado = READY;

    log_info(logger, "PID: <%u> - Estado Anterior: <NEW> - Estado Actual: <READY>", pcb->pid);

    sem_wait(&mutex_cola_ready);
        queue_push(cola_ready, (void*) pcb);
        char* lista_ready = string_aplanar_PID(cola_ready->elements);
        log_info(logger,"Ingreso a Cola Ready <COMUN>: %s", lista_ready);
        free(lista_ready);
    sem_post(&mutex_cola_ready);

    sem_post(&sem_procesos_esperando_en_ready);
}

void func_corto_plazo(void* arg){
    uint32_t pid;
    uint32_t quantum;
    bool cola_ready_plus_vacia;

    while (procesar_conexion_en_ejecucion){
        sem_wait(&sem_procesos_esperando_en_ready);

        if(sistema_detenido){
            stop_corto_plazo = true;
            log_info(logger, "STOP CORTO PLAZO");
            sem_wait(&sem_stop_corto_plazo);
            log_info(logger, "RESUME CORTO PLAZO");
        }

        // si un proceso en ready justo fue mandado a exit?, habria que salir de la funcion

        if(algoritmo_elegido == FIFO){
            sem_wait(&sem_cpu_disponible);
            mover_ready_a_execute(&pid);
        }

        if(algoritmo_elegido == RR){
            sem_wait(&sem_cpu_disponible);
            mover_ready_a_execute(&pid);
    
            log_debug(logger, "PID: <%u> - Ejecutando con Quantum de (%d) milisegundos", pid, config->quantum);
            sleep_ms(config->quantum);

            sem_wait(&mutex_proceso_en_ejecucion);
            if(proceso_en_ejecucion){
                log_debug(logger, "avisando a la CPU que desaloje al proceso actual");
                sem_wait(&mutex_conexion_cpu_interrupt);
                    envio_generico_op_code(conexion_cpu_interrupt, DESALOJO);
                sem_post(&mutex_conexion_cpu_interrupt);
            }
            sem_post(&mutex_proceso_en_ejecucion);
        }

        if(algoritmo_elegido == VRR){
            sem_wait(&sem_cpu_disponible);

            sem_wait(&mutex_cola_ready_aux);
                cola_ready_plus_vacia = queue_is_empty(cola_ready_aux);
            sem_post(&mutex_cola_ready_aux);

            if(!cola_ready_plus_vacia){
                mover_ready_aux_a_execute(&pid, &quantum);
            }
            else{
                mover_ready_a_execute(&pid);
                quantum = (uint32_t)config->quantum; 
            }

            log_debug(logger, "PID: <%u> - Ejecutando con Quantum de (%d) milisegundos", pid, (int)quantum);
            sleep_ms((int)quantum);

            sem_wait(&mutex_proceso_en_ejecucion);
            if(proceso_en_ejecucion){
                log_debug(logger, "avisando a la CPU que desaloje al proceso actual");
                sem_wait(&mutex_conexion_cpu_interrupt);
                    envio_generico_op_code(conexion_cpu_interrupt, DESALOJO);
                sem_post(&mutex_conexion_cpu_interrupt);
            }
            sem_post(&mutex_proceso_en_ejecucion);
        }    
    }
}

void mover_ready_a_execute(uint32_t* pid){
    sem_wait(&mutex_cola_ready);
        t_PCB* pcb = (t_PCB*) queue_pop(cola_ready);
    sem_post(&mutex_cola_ready);

    pcb->estado = EXECUTE;
    *pid = pcb->pid;

    log_info(logger, "PID: <%u> - Estado Anterior: <READY> - Estado Actual: <EXECUTE>", pcb->pid);

    sem_wait(&mutex_cola_execute);
        queue_push(cola_execute, (void*) pcb);
    sem_post(&mutex_cola_execute);

    sem_wait(&mutex_conexion_cpu_dispatch);
        enviar_pcb(conexion_cpu_dispatch, pcb, EJECUTAR_PROCESO);
    sem_post(&mutex_conexion_cpu_dispatch);

    sem_wait(&mutex_proceso_en_ejecucion);
        proceso_en_ejecucion = true;
    sem_post(&mutex_proceso_en_ejecucion);
}

bool mover_execute_a_blocked(t_PCB* pcb_nueva){
    sem_wait(&mutex_cola_execute);
        if(finalizacion_execute_dentro_kernel){
            finalizacion_execute_dentro_kernel = false;
            t_PCB* tmp = (t_PCB*) queue_pop(cola_execute);
            liberar_PCB(tmp);

            log_debug(logger, "PID: <%u> - Finalizado antes de pasar a <BLOCKED>", pcb_nueva->pid);

            char* motivo = obtener_motivo_salida(SALIDA_INTERRUPTED_BY_USER, NULL);
            mandar_a_exit(pcb_nueva, motivo);
            free(motivo);

            sem_post(&mutex_cola_execute);
            return false;
        }

        t_PCB* pcb_vieja = (t_PCB*) queue_pop(cola_execute);
    sem_post(&mutex_cola_execute);

    t_PCB* pcb_actualizada = actualizar_contexto(pcb_nueva, pcb_vieja);

    pcb_actualizada->estado = BLOCKED;

    log_info(logger, "PID: <%u> - Estado Anterior: <EXECUTE> - Estado Actual: <BLOCKED>", pcb_actualizada->pid);

    sem_wait(&mutex_cola_blocked);
        queue_push(cola_blocked, (void*) pcb_actualizada);
    sem_post(&mutex_cola_blocked);

    return true;
}

t_PCB* actualizar_contexto(t_PCB* pcb_nueva, t_PCB* pcb_vieja){
    if(pcb_nueva->pid == pcb_vieja->pid){
       liberar_PCB(pcb_vieja);
    }
    return pcb_nueva; 
}

void mover_blocked_a_ready(int pid){
    sem_wait(&mutex_cola_blocked);
        t_PCB* pcb = buscar_pcb_por_pid_y_remover(pid, cola_blocked->elements);
    sem_post(&mutex_cola_blocked);

    pcb->estado = READY;

    log_info(logger, "PID: <%u> - Estado Anterior: <BLOCKED> - Estado Actual: <READY>", pcb->pid);

    sem_wait(&mutex_cola_ready);
        queue_push(cola_ready, (void*) pcb);
        char* lista_ready = string_aplanar_PID(cola_ready->elements);
        log_info(logger,"Ingreso a Cola Ready <COMUN>: %s", lista_ready);
        free(lista_ready);
    sem_post(&mutex_cola_ready);

    sem_post(&sem_procesos_esperando_en_ready);
}

bool mover_execute_a_ready(t_PCB* pcb_nueva){
    sem_wait(&mutex_cola_execute);
        if(finalizacion_execute_dentro_kernel){
            finalizacion_execute_dentro_kernel = false;
            t_PCB* tmp = (t_PCB*) queue_pop(cola_execute);
            liberar_PCB(tmp);

            log_debug(logger, "PID: <%u> - Finalizado antes de pasar a <READY>", pcb_nueva->pid);

            char* motivo = obtener_motivo_salida(SALIDA_INTERRUPTED_BY_USER, NULL);
            mandar_a_exit(pcb_nueva, motivo);
            free(motivo);

            sem_post(&mutex_cola_execute);
            return false;
        }
        t_PCB* pcb_vieja = (t_PCB*) queue_pop(cola_execute);
    sem_post(&mutex_cola_execute);

    t_PCB* pcb_actualizada = actualizar_contexto(pcb_nueva, pcb_vieja);

    pcb_actualizada->estado = READY;

    log_info(logger, "PID: <%u> - Estado Anterior: <EXECUTE> - Estado Actual: <READY>", pcb_actualizada->pid);

    sem_wait(&mutex_cola_ready);
        queue_push(cola_ready, (void*) pcb_actualizada);
        char* lista_ready = string_aplanar_PID(cola_ready->elements);
        log_info(logger,"Ingreso a Cola Ready <COMUN>: %s", lista_ready);
        free(lista_ready);
    sem_post(&mutex_cola_ready);

    sem_post(&sem_procesos_esperando_en_ready);

    return true;
}

bool mover_execute_a_ready_aux(t_PCB* pcb_nueva){
    sem_wait(&mutex_cola_execute);
        if(finalizacion_execute_dentro_kernel){
            finalizacion_execute_dentro_kernel = false;
            t_PCB* tmp = (t_PCB*) queue_pop(cola_execute);
            liberar_PCB(tmp);

            log_debug(logger, "PID: <%u> - Finalizado antes de pasar a <READY_AUX>", pcb_nueva->pid);

            char* motivo = obtener_motivo_salida(SALIDA_INTERRUPTED_BY_USER, NULL);
            mandar_a_exit(pcb_nueva, motivo);
            free(motivo);

            sem_post(&mutex_cola_execute);
            return false;
        }
        t_PCB* pcb_vieja = (t_PCB*) queue_pop(cola_execute);
    sem_post(&mutex_cola_execute);

    t_PCB* pcb_actualizada = actualizar_contexto(pcb_nueva, pcb_vieja);

    pcb_actualizada->estado = READY;

    log_info(logger, "PID: <%u> - Estado Anterior: <EXECUTE> - Estado Actual: <READY_AUX>", pcb_actualizada->pid);

    sem_wait(&mutex_cola_ready_aux);
        queue_push(cola_ready_aux, (void*) pcb_actualizada);
        char* lista_ready_aux = string_aplanar_PID(cola_ready_aux->elements);
        log_info(logger,"Ingreso a Cola Ready <AUX>: %s", lista_ready_aux);
        free(lista_ready_aux);
    sem_post(&mutex_cola_ready_aux);

    sem_post(&sem_procesos_esperando_en_ready);

    return true;
}

void mover_ready_aux_a_execute(uint32_t* pid, uint32_t* quantum_restante){
    sem_wait(&mutex_cola_ready_aux);
        t_PCB* pcb = (t_PCB*) queue_pop(cola_ready_aux);
    sem_post(&mutex_cola_ready_aux);

    pcb->estado = EXECUTE;

    *quantum_restante = pcb->quantum;
    *pid = pcb->pid;

    log_info(logger, "PID: <%u> - Estado Anterior: <READY_AUX> - Estado Actual: <EXECUTE>", pcb->pid);

    sem_wait(&mutex_cola_execute);
        queue_push(cola_execute, (void*) pcb);
    sem_post(&mutex_cola_execute);

    sem_wait(&mutex_conexion_cpu_dispatch);
        enviar_pcb(conexion_cpu_dispatch, pcb, EJECUTAR_PROCESO);
    sem_post(&mutex_conexion_cpu_dispatch);

    sem_wait(&mutex_proceso_en_ejecucion);
        proceso_en_ejecucion = true;
    sem_post(&mutex_proceso_en_ejecucion);
}

void mover_blocked_a_ready_aux(int pid){
    sem_wait(&mutex_cola_blocked);
        t_PCB* pcb = buscar_pcb_por_pid_y_remover(pid, cola_blocked->elements);
    sem_post(&mutex_cola_blocked);

    pcb->estado = READY;

    log_info(logger, "PID: <%u> - Estado Anterior: <BLOCKED> - Estado Actual: <READY_AUX>", pcb->pid);

    sem_wait(&mutex_cola_ready_aux);
        queue_push(cola_ready_aux, (void*) pcb);
        char* lista_ready_aux = string_aplanar_PID(cola_ready_aux->elements);
        log_info(logger,"Ingreso a Cola Ready <AUX>: %s", lista_ready_aux);
        free(lista_ready_aux);
    sem_post(&mutex_cola_ready_aux);

    sem_post(&sem_procesos_esperando_en_ready);
}

void mandar_a_exit(t_PCB* pcb, char* motivo){

    char* estado_anterior = estado_to_string(pcb);
    pcb->estado = EXIT;

    if (motivo != NULL){
        log_info(logger, "Finaliza el proceso PID: <%u> - Motivo: <%s>", pcb->pid, motivo);
    }

    sem_wait(&mutex_conexion_memoria);;
        log_debug(logger, "Solicitando a MEMORIA que libere estructuras asocidas al proceso PID: <%u>", pcb->pid);
        enviar_generico_entero(conexion_memoria, PROCESO_FINALIZADO_MEMORIA, pcb->pid);
        validar_respuesta_op_code(conexion_memoria, MEMORIA_OK, logger);
    sem_post(&mutex_conexion_memoria);

    if(existe_recursos){
        sem_wait(&mutex_diccionario_recursos);
            log_debug(logger, "Liberando recursos retenidos por el proceso PID: <%u>", pcb->pid);
            devolver_recursos(pcb->pid);
        sem_post(&mutex_diccionario_recursos);
    }

    log_info(logger, "PID: <%u> - Estado Anterior: <%s> - Estado Actual: <EXIT>", pcb->pid, estado_anterior);

    /* 
        deberiamos liberar el pcb, pero por el momento lo guardamos, para luego poder mostrarlo 
        por la consola, o tambien liberar el pcb y almenos quedarnos con el PID
    */

    sem_wait(&mutex_cola_exit);
        queue_push(cola_exit, (void*) pcb);
    sem_post(&mutex_cola_exit);
}

bool mover_execute_a_exit(t_PCB* pcb_nueva, char* motivo){
    sem_wait(&mutex_cola_execute);
        if(finalizacion_execute_dentro_kernel){
            finalizacion_execute_dentro_kernel = false;
            t_PCB* tmp = (t_PCB*) queue_pop(cola_execute);
            liberar_PCB(tmp);

            log_debug(logger, "PID: <%u> - Finalizado antes de pasar a <EXIT>", pcb_nueva->pid);

            char* motivo = obtener_motivo_salida(SALIDA_INTERRUPTED_BY_USER, NULL);
            mandar_a_exit(pcb_nueva, motivo);
            free(motivo);

            sem_post(&mutex_cola_execute);
            return false;
        }       
        t_PCB* pcb_vieja = (t_PCB*) queue_pop(cola_execute);
    sem_post(&mutex_cola_execute);

    t_PCB* pcb_actualizada = actualizar_contexto(pcb_nueva, pcb_vieja); 

    mandar_a_exit(pcb_actualizada, motivo);
    
    return true;
}

void mover_blocked_a_exit(int pid, char* motivo){
    sem_wait(&mutex_cola_blocked);
        t_PCB* pcb_a_finalizar = buscar_pcb_por_pid_y_remover((int)pid, cola_blocked->elements);
    sem_post(&mutex_cola_blocked);

    //t_PCB* pcb_actualizada = actualizar_contexto(pcb_nueva, pcb_vieja); 

    mandar_a_exit(pcb_a_finalizar, motivo);
}

void devolver_a_execute(t_PCB* pcb){
    // hacemos esto para maniupularlo luego dentro del hilo
    sem_wait(&mutex_cola_execute);
        t_PCB* pcb_viejo = (t_PCB*) queue_pop(cola_execute); // sacamos la vieja
        t_PCB* pcb_actualizado = actualizar_contexto(pcb, pcb_viejo);
        queue_push(cola_execute, (void*) pcb_actualizado); // metemos la nueva
    sem_post(&mutex_cola_execute);

    // crear un hilo detach, asi ssi es necesario dormir se durme el hilo y no el planificador general
    // dentro del hilo se tiene un codigo similar al corto plazo 
    // (en este hilo sacamos el proceso de execute hacemos lo que hay que hacer y lo volvemos a execute)

    pthread_t hilo_mini_planificador_corto_plazo;
    pthread_create(&hilo_mini_planificador_corto_plazo, NULL, (void*) mini_planificador_corto_plazo, NULL);
    pthread_detach(hilo_mini_planificador_corto_plazo);
}

void mini_planificador_corto_plazo(void* arg){
    uint32_t pid;
    uint32_t quantum;

    if(algoritmo_elegido == FIFO){
        mover_execute_a_execute(&pid, &quantum);
    }

    if(algoritmo_elegido == RR){
        mover_execute_a_execute(&pid, &quantum);

        // como volvio de execute y regresa a execute nose si tiene que empezar con el Quamtum base o seguir con el Restante.
        log_debug(logger, "PID: <%u> - Ejecutando con Quantum de (%d) milisegundos", pid, config->quantum);
        sleep_ms(config->quantum);

        sem_wait(&mutex_proceso_en_ejecucion);
        if(proceso_en_ejecucion){
            log_debug(logger, "avisando a la CPU que desaloje al proceso actual");
            sem_wait(&mutex_conexion_cpu_interrupt);
                envio_generico_op_code(conexion_cpu_interrupt, DESALOJO);
            sem_post(&mutex_conexion_cpu_interrupt);
        }
        sem_post(&mutex_proceso_en_ejecucion);
    }

    if(algoritmo_elegido == VRR){
        mover_execute_a_execute(&pid, &quantum);

        log_debug(logger, "PID: <%u> - Ejecutando con Quantum de (%d) milisegundos", pid, (int)quantum);
        sleep_ms((int)quantum);

        sem_wait(&mutex_proceso_en_ejecucion);
        if(proceso_en_ejecucion){
            log_debug(logger, "avisando a la CPU que desaloje al proceso actual");
            sem_wait(&mutex_conexion_cpu_interrupt);
                envio_generico_op_code(conexion_cpu_interrupt, DESALOJO);
            sem_post(&mutex_conexion_cpu_interrupt);
        }
        sem_post(&mutex_proceso_en_ejecucion);
    }    
}

void mover_execute_a_execute(uint32_t* pid, uint32_t* quantum_restante){
    sem_wait(&mutex_cola_execute);
        t_PCB* pcb = (t_PCB*) queue_pop(cola_execute);
    sem_post(&mutex_cola_execute);

    pcb->estado = EXECUTE;

    *quantum_restante = pcb->quantum;
    *pid = pcb->pid;

    log_info(logger, "PID: <%u> - Estado Anterior: <EXECUTE> - Estado Actual: <EXECUTE>", pcb->pid);

    sem_wait(&mutex_cola_execute);
        queue_push(cola_execute, (void*) pcb);
    sem_post(&mutex_cola_execute);

    sem_wait(&mutex_conexion_cpu_dispatch);
        enviar_pcb(conexion_cpu_dispatch, pcb, EJECUTAR_PROCESO);
    sem_post(&mutex_conexion_cpu_dispatch);

    sem_wait(&mutex_proceso_en_ejecucion);
        proceso_en_ejecucion = true;
    sem_post(&mutex_proceso_en_ejecucion);
}

void mover_a_ready_o_ready_aux(t_PCB* pcb){
    if((algoritmo_elegido == VRR) && (pcb->quantum != 0)){
        pcb->estado = READY;

        log_info(logger, "PID: <%u> - Estado Anterior: <BLOCKED_RECURSO> - Estado Actual: <READY_AUX>", pcb->pid);

        sem_wait(&mutex_cola_ready_aux);
            queue_push(cola_ready_aux, (void*) pcb);
            char* lista_ready_aux = string_aplanar_PID(cola_ready_aux->elements);
            log_info(logger,"Ingreso a Cola Ready <AUX>: %s", lista_ready_aux);
            free(lista_ready_aux);
        sem_post(&mutex_cola_ready_aux);

        sem_post(&sem_procesos_esperando_en_ready);
    }
    else{
        pcb->estado = READY;

        log_info(logger, "PID: <%u> - Estado Anterior: <BLOCKED_RECURSO> - Estado Actual: <READY>", pcb->pid);

        sem_wait(&mutex_cola_ready);
            queue_push(cola_ready, (void*) pcb);
            char* lista_ready = string_aplanar_PID(cola_ready->elements);
            log_info(logger,"Ingreso a Cola Ready <Comun>: %s", lista_ready);
            free(lista_ready);
        sem_post(&mutex_cola_ready);

        sem_post(&sem_procesos_esperando_en_ready);
    }  
}


void agregar_registro_recurso(uint32_t pid, char* recurso){
    t_registro_recurso* tmp = malloc(sizeof(t_registro_recurso));

    tmp->pid = pid;
    tmp->recurso = strdup(recurso);
    tmp->devuelto = false;

    log_info(logger,"PID: <%u> - consumiendo recurso (%s)", tmp->pid, tmp->recurso);
    list_add(recursos_asignados, (void*) tmp);
}

void eliminar_registro_recurso(uint32_t pid, char* recurso){
    int size_lista = list_size(recursos_asignados);
    t_registro_recurso* registro_recurso = NULL;

    if(size_lista == 0){
        return;
    }

    for(int i = 0; i < size_lista; i++){
        registro_recurso = (t_registro_recurso*) list_get(recursos_asignados, i);
        if((registro_recurso->pid == pid) && (strcmp(registro_recurso->recurso, recurso) == 0)){
            registro_recurso = (t_registro_recurso*) list_remove(recursos_asignados, i);
            log_info(logger,"PID: <%u> - Devolviendo recurso (%s)", registro_recurso->pid, registro_recurso->recurso);
            liberar_elemento_t_registro_recurso((void*) registro_recurso);
            return;
        }
    }
}

void devolver_recursos(uint32_t pid){
    int size_lista = list_size(recursos_asignados);
    t_registro_recurso* registro_recurso = NULL;
    t_recurso* recurso = NULL;
    t_PCB* pcb = NULL;

    if(size_lista == 0){
        return;
    }
    
    for(int i = 0; i < size_lista; i++){
        registro_recurso = (t_registro_recurso*) list_get(recursos_asignados, i);
        
        if((registro_recurso->pid == pid) && (!registro_recurso->devuelto)){
            recurso = (t_recurso*) dictionary_get(recursos, registro_recurso->recurso);

            recurso->instancias += 1;

            registro_recurso->devuelto = true;

            if(recurso->instancias > 0){
                pcb = (t_PCB*) queue_pop(recurso->cola_recurso);
                
                // mandar el proceso recien desbloqueado a Ready o Ready plus (segun el algortimo)
                log_info(logger, "PID: <%u> - LIBERA: (%s) - Instancias: (%d) - Desbloquea a PID <%u>", pid, registro_recurso->recurso, recurso->instancias, pcb->pid);
                mover_a_ready_o_ready_aux(pcb);
            }
        }
    }

    eliminar_devueltos();
}

void eliminar_devueltos(void){
    bool condicion (void* elemento){
        t_registro_recurso* tmp = (t_registro_recurso*) elemento;
        return tmp->devuelto;
    }
    list_remove_and_destroy_all_by_condition(recursos_asignados, condicion, liberar_elemento_t_registro_recurso);
}



/////////////////////////////////////////////////////////////////////////////////////
char* obtener_motivo_salida(motivo_exit motivo, const char* nombre){
    char* motivo_str = NULL;
    const char* base_str = NULL;

    switch (motivo) {
        case SALIDA_SUCCESS:
            base_str = "SUCCESS";
            break;
        case SALIDA_INVALID_RESOURCE:
            base_str = "INVALID_RESOURCE";
            break;
        case SALIDA_SEGMENTATION_FAULT:
            base_str = "SEGMENTATION_FAULT";
            break;
        case SALIDA_OUT_OF_MEMORY:
            base_str = "OUT_OF_MEMORY";
            break;
        case SALIDA_INVALID_INTERFACE:
            base_str = "INVALID_INTERFACE";
            break;
        case SALIDA_INVALID_INTERFACE_OPERATION:
            base_str = "INVALID_INTERFACE_OPERATION";
            break;
        case SALIDA_INTERRUPTED_BY_USER:
            base_str = "INTERRUPTED_BY_USER";
            break;
        default:
            base_str = "UNKNOWN";
            break;
    }

    // Si nombre no es NULL, concatenar base_str y nombre
    if (nombre != NULL) {
        // Calcular la longitud total del string final
        size_t len_base = strlen(base_str);
        size_t len_nombre = strlen(nombre);
        size_t total_len = len_base + len_nombre + 2; // +2 para espacio y '\0'

        // Asignar memoria para el string final
        motivo_str = (char*)malloc(total_len * sizeof(char));
        if (motivo_str != NULL) {
            snprintf(motivo_str, total_len, "%s (%s)", base_str, nombre);
        }
    } else {
        // Si nombre es NULL, solo copiar base_str
        motivo_str = strdup(base_str); // strdup asigna memoria y copia el string
    }

    return motivo_str;
}