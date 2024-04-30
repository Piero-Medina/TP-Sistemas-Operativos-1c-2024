#include "ciclo_de_instruccion.h"

void ejecutar_ciclo_de_instruccion(int conexion, t_PCB* pcb){
    struct timeval inicio;
    //struct timeval final;
    gettimeofday(&inicio, NULL);
    t_instruccion* instruccion = NULL;
    
    do {
        log_info(logger, "PID: <%d> - FETCH - Program Counter: <%d>", pcb->pid, pcb->program_counter);
        
        perdir_intruccion_a_memoria(conexion_memoria, SOLICITAR_INTRUCCION_MEMORIA, pcb->pid, pcb->program_counter);
        instruccion = recibir_instruccion(conexion);
        
        pcb->program_counter++;
        pcb->registros->PC++;
        
        switch (instruccion->identificador){
            case SET:
                char* registro_set = (char*) list_get(instruccion->parametros, 0);
                int valor_set = atoi((char*) list_get(instruccion->parametros, 1));

                log_info(logger, "PID: <%d> - Ejecutando: <SET> - <%s> - <%d>", pcb->pid, registro_set, valor_set);
                
                registro registro_a_setear = obtener_registro_por_nombre(registro_set);
                set_registro(pcb, valor_set, registro_a_setear);

                destruir_elemento_instruccion((void*) instruccion);
                break;
            case SUM:
                log_info(logger, "PID: <%d> - Ejecutando: <SET> - <PARAMETROS>", pcb->pid);
                break;
            case SUB:
                log_info(logger, "PID: <%d> - Ejecutando: <SET> - <PARAMETROS>", pcb->pid);
                break;
            case JNZ:
                log_info(logger, "PID: <%d> - Ejecutando: <SET> - <PARAMETROS>", pcb->pid);
                break;
            case IO_GEN_SLEEP:
                log_info(logger, "PID: <%d> - Ejecutando: <SET> - <PARAMETROS>", pcb->pid);
                break;
            default:
                break;
            
            // verificar interrupcion

        }
    } while (1);

}


// operacion para el final de ejecucion
// int tiempo_ejecucion_ms = (end_t.tv_sec - start_t.tv_sec) * 1000;