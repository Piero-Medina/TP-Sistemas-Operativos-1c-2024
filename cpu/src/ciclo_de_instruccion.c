#include "ciclo_de_instruccion.h"

/*
    si estamos aca el socket disponible es el que establece 
    la conexion entre (kernel - cpu dispatch).
*/

void ejecutar_ciclo_de_instruccion(int conexion, t_PCB* pcb){
    struct timeval inicio;
    struct timeval final;
    gettimeofday(&inicio, NULL);
    t_instruccion* instruccion = NULL;
    bool puede_seguir_ejecutando = true;
    
    do {
        log_info(logger, "PID: <%d> - FETCH - Program Counter: <%d>", pcb->pid, pcb->program_counter);
        
        solicitar_intruccion_a_memoria(conexion_memoria, SOLICITAR_INTRUCCION_MEMORIA, pcb->pid, pcb->program_counter);
        ignorar_op_code(conexion_memoria);
        instruccion = recibir_instruccion(conexion_memoria);
 
        switch (instruccion->identificador){
            case SET:
                char* registro_set = (char*) list_get(instruccion->parametros, 0);
                int valor_set = atoi((char*) list_get(instruccion->parametros, 1));

                log_info(logger, "PID: <%d> - Ejecutando: <SET> - <%s> - <%d>", pcb->pid, registro_set, valor_set);
                
                registro e_registro_set = obtener_registro_por_nombre(registro_set);
                set_registro(pcb, valor_set, e_registro_set);

                destruir_elemento_instruccion((void*) instruccion);
                break;
            case SUM:
                log_info(logger, "PID: <%d> - Ejecutando: <SUM> - <PARAMETROS>", pcb->pid);
                break;
            case SUB:
                log_info(logger, "PID: <%d> - Ejecutando: <SUB> - <PARAMETROS>", pcb->pid);
                break;
            case JNZ:
                log_info(logger, "PID: <%d> - Ejecutando: <JNZ> - <PARAMETROS>", pcb->pid);
                break;
            case IO_GEN_SLEEP:
                log_info(logger, "PID: <%d> - Ejecutando: <IO_GEN_SLEEP> - <PARAMETROS>", pcb->pid);
                break;
            default:
                log_info(logger, "PID: <%d> - No se reconoce Instruccion", pcb->pid);
                break;
        }

        incrementar_program_counter(pcb, 1);
            
        log_info(logger, "PID: <%d> - CHECK INTERRUPT (verificando) ", pcb->pid);
        sem_wait(&mutex_desalojo);
            if(desalojo){
                log_info(logger, "PID: <%d> - CHECK INTERRUPT (true)", pcb->pid);

                puede_seguir_ejecutando = false;
                gettimeofday(&final, NULL);
                establecer_tiempo_restante_de_ejecucion(pcb, inicio, final);

                enviar_pcb(conexion, pcb, DESALOJO);
                    
                desalojo = false;
            }
            else{
                log_info(logger, "PID: <%d> - CHECK INTERRUPT (false)", pcb->pid);
            }
        sem_post(&mutex_desalojo);

    } while (puede_seguir_ejecutando);

}

void establecer_tiempo_restante_de_ejecucion(t_PCB* pcb,struct timeval inicio, struct timeval final){
    int tiempo_ejecucion_ms = (final.tv_sec - inicio.tv_sec) * 1000;
    int tiempo_restante_ms = pcb->quantum - tiempo_ejecucion_ms;

    /*
        puede pasar que sea menor a cero, dado que las interrupciones en el ciclo de instruccion
        las revisamos una vez terminada de ejecutar la intruccion
    */
    if(tiempo_restante_ms <= 0){
        pcb->quantum = 0;
    }
    else{
        pcb->quantum -= tiempo_ejecucion_ms;
    }
}


void incrementar_program_counter(t_PCB* pcb, int en_cuanto){
    pcb->program_counter += en_cuanto;
    pcb->registros->PC += en_cuanto;
}
