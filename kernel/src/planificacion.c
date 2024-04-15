#include "planificacion.h"

void mover_a_new(t_PCB* pcb){
    sem_wait(&mutex_cola_new);
        queue_push(cola_new, (void*) pcb);
    sem_post(&mutex_cola_new);

    log_info(logger, "Se crea el proceso <%d> en NEW", pcb->pid);
}


void func_largo_plazo(void* arg){
    sem_wait(&sem_grado_multiprogramacion);
    // mover_new_a_ready()
}