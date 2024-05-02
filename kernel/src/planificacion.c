#include "planificacion.h"

void mover_a_new(t_PCB* pcb){
    sem_wait(&mutex_cola_new);
        queue_push(cola_new, (void*) pcb);
    sem_post(&mutex_cola_new);

    log_info(logger, "Se crea el proceso PID: <%d> en NEW", pcb->pid);
    
    sem_post(&sem_procesos_esperando_en_new);

}

void func_largo_plazo(void* arg){
    while (1){
        sem_wait(&sem_procesos_esperando_en_new);
        sem_wait(&sem_grado_multiprogramacion);
        mover_new_a_ready();
    }
}

void mover_new_a_ready(void){
    sem_wait(&mutex_cola_new);
        t_PCB* pcb = (t_PCB*) queue_pop(cola_new);
    sem_post(&mutex_cola_new);

    pcb->estado = READY;

    log_info(logger, "PID: <%d> - Estado Anterior: <NEW> - Estado Actual: <READY>", pcb->pid);

    sem_wait(&mutex_cola_ready);
        queue_push(cola_ready, (void*) pcb);
        char* lista_ready = string_aplanar_PID(cola_ready->elements);
        log_info(logger,"Ingreso a Cola Ready <Comun>: %s", lista_ready);
        free(lista_ready);
    sem_post(&mutex_cola_ready);

    sem_post(&sem_procesos_esperando_en_ready);
}

void func_corto_plazo(void* arg){
    while (1){
        sem_wait(&sem_procesos_esperando_en_ready);

        if(algoritmo_elegido == FIFO){
            sem_wait(&sem_cpu_disponible);
            mover_ready_a_execute();
        }

        if(algoritmo_elegido == RR){
            sem_wait(&sem_cpu_disponible);
            mover_ready_a_execute();

            sleep_ms(config->quantum);

            sem_wait(&mutex_proceso_en_ejecucion);
            if(proceso_en_ejecucion){
                // IIIIII -> luego revisar si puede haber otros motivos de desalojo
                log_info(logger, "avisando a la CPU que desaloje el proceso actual");
                sem_wait(&mutex_conexion_cpu_interrupt);
                    envio_generico_op_code(conexion_cpu_interrupt, DESALOJO);
                sem_post(&mutex_conexion_cpu_interrupt);
            }
            sem_post(&mutex_proceso_en_ejecucion);
        }
        
    }
}

void mover_ready_a_execute(void){
    sem_wait(&mutex_cola_ready);
        t_PCB* pcb = (t_PCB*) queue_pop(cola_ready);
    sem_post(&mutex_cola_ready);

    pcb->estado = EXECUTE;

    log_info(logger, "PID: <%d> - Estado Anterior: <READY> - Estado Actual: <EXECUTE>", pcb->pid);

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

void mover_execute_a_blocked(t_PCB* pcb_nueva){
    sem_wait(&mutex_cola_execute);
        t_PCB* pcb_vieja = (t_PCB*) queue_pop(cola_execute);
    sem_post(&mutex_cola_execute);

    t_PCB* pcb_actualizada = actualizar_contexto(pcb_nueva, pcb_vieja);

    pcb_actualizada->estado = BLOCKED;

    log_info(logger, "PID: <%d> - Estado Anterior: <EXECUTE> - Estado Actual: <BLOCKED>", pcb_actualizada->pid);

    sem_wait(&mutex_cola_blocked);
        queue_push(cola_blocked, (void*) pcb_actualizada);
    sem_post(&mutex_cola_blocked);
}

t_PCB* actualizar_contexto(t_PCB* pcb_nueva, t_PCB* pcb_vieja){
    if(pcb_nueva->pid == pcb_vieja->pid){
       liberar_PCB(pcb_vieja);
    }
    return pcb_nueva; 
}

void mover_blocked_a_ready(void){
    sem_wait(&mutex_cola_blocked);
        t_PCB* pcb = (t_PCB*) queue_pop(cola_blocked);
    sem_post(&mutex_cola_blocked);

    pcb->estado = READY;

    log_info(logger, "PID: <%d> - Estado Anterior: <BLOCKED> - Estado Actual: <READY>", pcb->pid);

    sem_wait(&mutex_cola_ready);
        queue_push(cola_ready, (void*) pcb);
        char* lista_ready = string_aplanar_PID(cola_ready->elements);
        log_info(logger,"Ingreso a Cola Ready <Comun>: %s", lista_ready);
        free(lista_ready);
    sem_post(&mutex_cola_ready);

    sem_post(&sem_procesos_esperando_en_ready);
} 

void mandar_a_exit(t_PCB* pcb){

    char* estado_anterior = estado_to_string(pcb);
    pcb->estado = EXIT;

    sem_wait(&mutex_conexion_memoria);
        //enviar_pcb(conexion_memoria, pcb, PROCESO_FINALIZADO_MEMORIA);
        log_info(logger, "Solicitando a MEMORIA que libere estructuras asocidas al proceso PID: <%d>", pcb->pid);
        envio_generico_entero(conexion_memoria, PROCESO_FINALIZADO_MEMORIA, pcb->pid);
        validar_respuesta_op_code(conexion_memoria, MEMORIA_OK, logger);
    sem_post(&mutex_conexion_memoria);

    log_info(logger, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <EXIT>", pcb->pid, estado_anterior);

    /* 
        deberiamos liberar el pcb, pero por el momento lo guardamos, para luego poder mostrarlo 
        por la consola, o tambien liberar el pcb y almenos quedarnos con el PID
    */
    sem_wait(&mutex_cola_exit);
        queue_push(cola_exit, (void*) pcb);
    sem_post(&mutex_cola_exit);
}

void mover_execute_a_ready(t_PCB* pcb_nueva){
    sem_wait(&mutex_cola_execute);
        t_PCB* pcb_vieja = (t_PCB*) queue_pop(cola_execute);
    sem_post(&mutex_cola_execute);

    t_PCB* pcb_actualizada = actualizar_contexto(pcb_nueva, pcb_vieja);

    pcb_actualizada->estado = READY;

    log_info(logger, "PID: <%d> - Estado Anterior: <EXECUTE> - Estado Actual: <READY>", pcb_actualizada->pid);

    sem_wait(&mutex_cola_ready);
        queue_push(cola_ready, (void*) pcb_actualizada);
        char* lista_ready = string_aplanar_PID(cola_ready->elements);
        log_info(logger,"Ingreso a Cola Ready <Comun>: %s", lista_ready);
        free(lista_ready);
    sem_post(&mutex_cola_ready);

    sem_post(&sem_procesos_esperando_en_ready);
}

void mover_execute_a_exit(t_PCB* pcb_nueva){
    sem_wait(&mutex_cola_execute);
        t_PCB* pcb_vieja = (t_PCB*) queue_pop(cola_execute);
    sem_post(&mutex_cola_execute);

    t_PCB* pcb_actualizada = actualizar_contexto(pcb_nueva, pcb_vieja); 

    mandar_a_exit(pcb_actualizada);
}