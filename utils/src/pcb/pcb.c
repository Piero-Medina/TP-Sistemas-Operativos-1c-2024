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

registro obtener_registro_por_nombre(char *nombre){
    if (strcmp(nombre, "AX") == 0) {
        return AX;
    } else if (strcmp(nombre, "BX") == 0) {
        return BX;
    } else if (strcmp(nombre, "CX") == 0) {
        return CX;
    } else if (strcmp(nombre, "DX") == 0) {
        return DX;
    } else if (strcmp(nombre, "EAX") == 0) {
        return EAX;
    } else if (strcmp(nombre, "EBX") == 0) {
        return EBX;
    } else if (strcmp(nombre, "ECX") == 0) {
        return ECX;
    } else if (strcmp(nombre, "EDX") == 0) {
        return EDX;
    } else if (strcmp(nombre, "SI") == 0) {
        return SI;
    } else if (strcmp(nombre, "DI") == 0) {
        return DI;
    } else if (strcmp(nombre, "PC") == 0) {
        return PC;
    } else {
        // Si no se encuentra el nombre del registro, se devuelve un valor inválido
        return -1;
    }
}

void set_registro(t_PCB *pcb, uint32_t valor, registro registro){
    switch (registro) {
        case AX:
            pcb->registros->AX = valor;
            break;
        case BX:
            pcb->registros->BX = valor;
            break;
        case CX:
            pcb->registros->CX = valor;
            break;
        case DX:
            pcb->registros->DX = valor;
            break;
        case EAX:
            pcb->registros->EAX = valor;
            break;
        case EBX:
            pcb->registros->EBX = valor;
            break;
        case ECX:
            pcb->registros->ECX = valor;
            break;
        case EDX:
            pcb->registros->EDX = valor;
            break;
        case SI:
            pcb->registros->SI = valor;
            break;
        case DI:
            pcb->registros->DI = valor;
            break;
        case PC:
            pcb->registros->PC = valor;
            break;
        default:
            printf("Error: Índice de registro inválido.\n");
    }
}


uint32_t get_registro(t_PCB *pcb, registro registro) {
    uint32_t valor;

    switch (registro) {
        case AX:
            valor = pcb->registros->AX;
            break;
        case BX:
            valor = pcb->registros->BX;
            break;
        case CX:
            valor = pcb->registros->CX;
            break;
        case DX:
            valor = pcb->registros->DX;
            break;
        case EAX:
            valor = pcb->registros->EAX;
            break;
        case EBX:
            valor = pcb->registros->EBX;
            break;
        case ECX:
            valor = pcb->registros->ECX;
            break;
        case EDX:
            valor = pcb->registros->EDX;
            break;
        case SI:
            valor = pcb->registros->SI;
            break;
        case DI:
            valor = pcb->registros->DI;
            break;
        case PC:
            valor = pcb->registros->PC;
            break;
        default:
            printf("Error: Índice de registro inválido.\n");
            return 0;
    }

    return valor;
}


