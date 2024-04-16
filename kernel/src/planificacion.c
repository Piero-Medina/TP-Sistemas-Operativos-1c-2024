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

        sem_wait(&sem_cpu_disponible);
        mover_ready_a_execute();
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
        // enviar pcb a cpu para ejecutar
    sem_post(&mutex_conexion_cpu_dispatch);
}

