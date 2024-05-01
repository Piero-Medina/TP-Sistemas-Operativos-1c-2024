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
    
    return list_find(lista_de_procesos, coincide_con_pid);
}