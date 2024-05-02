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
    bool proceso_sigue_en_cpu = true;
    
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
                
                e_registro e_registro_set = obtener_registro_por_nombre(registro_set);
                set_registro(pcb, valor_set, e_registro_set);
                break;
            case SUM:
                char* registro_dest_sum = (char*) list_get(instruccion->parametros, 0);
                char* registro_orig_sum = (char*) list_get(instruccion->parametros, 1);

                log_info(logger, "PID: <%d> - Ejecutando: <SUM> - <%s> - <%s>", pcb->pid, registro_dest_sum, registro_orig_sum);

                int suma_sum = get_registro(pcb, obtener_registro_por_nombre(registro_dest_sum)) +
                                get_registro(pcb, obtener_registro_por_nombre(registro_orig_sum));

                set_registro(pcb, suma_sum, obtener_registro_por_nombre(registro_dest_sum));
                break;
            case SUB:
                char* registro_dest_sub = (char*) list_get(instruccion->parametros, 0);
                char* registro_orig_sub = (char*) list_get(instruccion->parametros, 1);

                log_info(logger, "PID: <%d> - Ejecutando: <SUB> - <%s> - <%s>", pcb->pid, registro_dest_sub, registro_orig_sub);
                
                int resta_sub = get_registro(pcb, obtener_registro_por_nombre(registro_dest_sub)) -
                                get_registro(pcb, obtener_registro_por_nombre(registro_orig_sub));
                
                set_registro(pcb, resta_sub, obtener_registro_por_nombre(registro_dest_sub));
                break;
            case JNZ:
                char* registro_jnz = (char*) list_get(instruccion->parametros, 0);
                int valor_jnz = atoi((char*) list_get(instruccion->parametros, 1));
                
                log_info(logger, "PID: <%d> - Ejecutando: <JNZ> - <%s> - <%d>", pcb->pid, registro_jnz, valor_jnz);

                if(get_registro(pcb, obtener_registro_por_nombre(registro_jnz)) != 0){
                    pcb->program_counter = valor_jnz;
                    pcb->registros->PC = valor_jnz;
                } 
                break;
            case IO_GEN_SLEEP:
                char* interfaz_generica = (char*) list_get(instruccion->parametros, 0);
                int unidades_generica = atoi((char*) list_get(instruccion->parametros, 1));

                log_info(logger, "PID: <%d> - Ejecutando: <IO_GEN_SLEEP> - <%s> - <%d>", pcb->pid, interfaz_generica, unidades_generica);
                
                // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
                incrementar_program_counter(pcb, 1); 

                log_info(logger, "PID: <%d> - se va de CPU", pcb->pid);
                enviar_pcb(conexion, pcb, PETICION_IO);
                //envio_generico_string(conexion, GENERICA, interfaz_generica);
                //envio_generico_entero(conexion, IO_GEN_SLEEP, unidades_generica);
                puede_seguir_ejecutando = false;
                proceso_sigue_en_cpu = false;
                break;
            case EXIT_I:
                log_info(logger, "PID: <%d> - Ejecutando: <EXIT> ", pcb->pid);

                // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
                incrementar_program_counter(pcb, 1); 

                log_info(logger, "PID: <%d> - se va de CPU", pcb->pid);
                enviar_pcb(conexion, pcb, PROCESO_FINALIZADO);
                puede_seguir_ejecutando = false;
                proceso_sigue_en_cpu = false;
                break;
            default:
                log_info(logger, "PID: <%d> - No se reconoce Instruccion", pcb->pid);
                break;
        }

        // no incrementamos el PC ante un JNZ ya que este setea un nuevo valor para PC 
        if(instruccion->identificador != JNZ) 
        incrementar_program_counter(pcb, 1);
            
        log_info(logger, "PID: <%d> - CHECK INTERRUPT (verificando) ", pcb->pid);
        sem_wait(&mutex_desalojo);
            if(desalojo){
                log_info(logger, "PID: <%d> - CHECK INTERRUPT (true)", pcb->pid);

                if(proceso_sigue_en_cpu){
                    puede_seguir_ejecutando = false;
                    gettimeofday(&final, NULL);
                    establecer_tiempo_restante_de_ejecucion(pcb, inicio, final);

                    log_info(logger, "PID: <%d> - se va de CPU", pcb->pid);
                    enviar_pcb(conexion, pcb, DESALOJO);
                    
                    desalojo = false;
                }
                else{
                    log_info(logger, "PID: <%d> - CHECK INTERRUPT (negado) - el proceso se fue de CPU", pcb->pid);
                    puede_seguir_ejecutando = false;

                    desalojo = false;
                }
            }
            else{
                log_info(logger, "PID: <%d> - CHECK INTERRUPT (false)", pcb->pid);
            }
        sem_post(&mutex_desalojo);

        destruir_elemento_instruccion((void*) instruccion);

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

