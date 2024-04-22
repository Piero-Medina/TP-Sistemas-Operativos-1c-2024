#include <pcb/pcb.h>

t_PCB* crear_PCB(int pid, int quantum, estado_pcb estado){
    t_PCB* nuevo_pcb = malloc(sizeof(t_PCB));

    nuevo_pcb->pid = pid;
    nuevo_pcb->program_counter = 0;
    nuevo_pcb->quantum = quantum;
    nuevo_pcb->registros = crear_registros_cpu();
    nuevo_pcb->estado = estado;

    return nuevo_pcb;
}

registros_cpu* crear_registros_cpu(){
    registros_cpu* registros = malloc(sizeof(registros_cpu));

    registros->PC = 0;
    registros->AX = 0;
    registros->BX = 0;
    registros->CX = 0;
    registros->DX = 0;
    registros->EAX = 0;
    registros->EBX = 0;
    registros->ECX = 0;
    registros->EDX = 0;
    registros->SI = 0;
    registros->DI = 0;
    
    return registros;
}

void liberar_registros_cpu(registros_cpu* registros){
    if (registros != NULL) {
        free(registros);
    }
}

void liberar_PCB(t_PCB* pcb){
    if (pcb != NULL) {
        liberar_registros_cpu(pcb->registros); 
        free(pcb);
    }
}

char* estado_to_string(t_PCB* pcb) {
    switch (pcb->estado) {
        case NEW:
            return "NEW";
        case READY:
            return "READY";
        case EXECUTE:
            return "EXECUTE";
        case BLOCKED:
            return "BLOCKED";
        case EXIT:
            return "EXIT";
        default:
            return "UNKNOWN";
    }
}
