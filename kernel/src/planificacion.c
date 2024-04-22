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
    sem_post(&mutex_cola_ready);

    sem_post(&sem_procesos_esperando_en_ready);
} 
