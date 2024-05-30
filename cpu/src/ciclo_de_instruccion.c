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
        log_info(logger, "PID: <%u> - FETCH - Program Counter: <%d>", pcb->pid, pcb->program_counter);
        
        solicitar_intruccion_a_memoria(conexion_memoria, SOLICITAR_INTRUCCION_MEMORIA, pcb->pid, pcb->program_counter);
        ignorar_op_code(conexion_memoria);
        instruccion = recibir_instruccion(conexion_memoria);
 
        switch (instruccion->identificador){
            case SET:
            {
                char* registro = (char*) list_get(instruccion->parametros, 0);
                int valor = atoi((char*) list_get(instruccion->parametros, 1));

                log_info(logger, "PID: <%u> - Ejecutando: <SET> - <%s> - <%d>", pcb->pid, registro, valor);

                e_registro e_registro = obtener_registro_por_nombre(registro);
                set_registro(pcb, (uint32_t)valor, e_registro);

                log_info(logger, "PID: <%u> - Finalizando: <SET> - (%s = %u)", pcb->pid, registro, (uint32_t)valor);
                break;
            }
            case SUM:
            {
                char* registro_dest = (char*) list_get(instruccion->parametros, 0);
                char* registro_orig = (char*) list_get(instruccion->parametros, 1);

                log_info(logger, "PID: <%u> - Ejecutando: <SUM> - <%s> - <%s>", pcb->pid, registro_dest, registro_orig);

                uint32_t suma = get_registro(pcb, obtener_registro_por_nombre(registro_dest)) +
                                get_registro(pcb, obtener_registro_por_nombre(registro_orig));

                set_registro(pcb, suma, obtener_registro_por_nombre(registro_dest));

                log_info(logger, "PID: <%u> - Finalizando: <SUM> (%s = %u)", pcb->pid, registro_dest, suma);
                break;
            }
            case SUB:
            {
                char* registro_dest = (char*) list_get(instruccion->parametros, 0);
                char* registro_orig = (char*) list_get(instruccion->parametros, 1);

                log_info(logger, "PID: <%u> - Ejecutando: <SUB> - <%s> - <%s>", pcb->pid, registro_dest, registro_orig);
                
                uint32_t resta = get_registro(pcb, obtener_registro_por_nombre(registro_dest)) -
                                     get_registro(pcb, obtener_registro_por_nombre(registro_orig));
                
                set_registro(pcb, resta, obtener_registro_por_nombre(registro_dest));

                log_info(logger, "PID: <%u> - Finalizando: <SUB> (%s = %u)", pcb->pid, registro_dest, resta);
                break;
            }
            case JNZ:
            {
                char* registro = (char*) list_get(instruccion->parametros, 0);
                int nro_instruccion = atoi((char*) list_get(instruccion->parametros, 1));
                
                log_info(logger, "PID: <%u> - Ejecutando: <JNZ> - <%s> - <%d>", pcb->pid, registro, nro_instruccion);

                if(get_registro(pcb, obtener_registro_por_nombre(registro)) != 0){
                    pcb->program_counter = (uint32_t)nro_instruccion;
                    pcb->registros->PC = (uint32_t)nro_instruccion;
                }

                log_info(logger, "PID: <%u> - Finalizando: <JNZ> (PC = %u)", pcb->pid, pcb->program_counter); 
                break;
            }
            case IO_GEN_SLEEP:
            {
                char* nombre_interfaz = (char*) list_get(instruccion->parametros, 0);
                int unidades_de_trabajo = atoi((char*) list_get(instruccion->parametros, 1));

                log_info(logger, "PID: <%u> - Ejecutando: <IO_GEN_SLEEP> - <%s> - <%d>", pcb->pid, nombre_interfaz, unidades_de_trabajo);
                
                // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
                incrementar_program_counter(pcb, 1);

                establecer_tiempo_restante_de_ejecucion(pcb, inicio, final); 

                log_info(logger, "PID: <%u> - Se va de CPU", pcb->pid);
                enviar_pcb(conexion, pcb, PETICION_IO);
                envio_generico_string(conexion, GENERICA, nombre_interfaz);
                envio_generico_entero(conexion, IO_GEN_SLEEP, (uint32_t)unidades_de_trabajo);
                puede_seguir_ejecutando = false;
                proceso_sigue_en_cpu = false;

                log_info(logger, "PID: <%u> - Finalizando: <IO_GEN_SLEEP> ", pcb->pid);
                break;
            }
            case EXIT_I:
            {
                log_info(logger, "PID: <%u> - Ejecutando: <EXIT> ", pcb->pid);

                // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
                incrementar_program_counter(pcb, 1);

                establecer_tiempo_restante_de_ejecucion(pcb, inicio, final); 

                log_info(logger, "PID: <%u> - Se va de CPU", pcb->pid);
                enviar_pcb(conexion, pcb, PROCESO_FINALIZADO);
                puede_seguir_ejecutando = false;
                proceso_sigue_en_cpu = false;

                log_info(logger, "PID: <%u> - Finalizando: <EXIT> ", pcb->pid);
                break;
            }
            default:
                log_info(logger, "PID: <%d> - No se reconoce Instruccion", pcb->pid);
                break;
        }

        // no incrementamos el PC ante un JNZ ya que este setea un nuevo valor para PC 
        if(instruccion->identificador != JNZ){ 
            incrementar_program_counter(pcb, 1);
        }
            
        log_info(logger, "PID: <%u> - CHECK INTERRUPT (verificando) ", pcb->pid);
        sem_wait(&mutex_desalojo);
            if(desalojo){
                log_info(logger, "PID: <%u> - CHECK INTERRUPT (True)", pcb->pid);

                if(proceso_sigue_en_cpu){
                    puede_seguir_ejecutando = false;

                    establecer_tiempo_restante_de_ejecucion(pcb, inicio, final);

                    log_info(logger, "PID: <%u> - Se va de CPU", pcb->pid);
                    enviar_pcb(conexion, pcb, DESALOJO);
                    
                    desalojo = false;
                }
                else{
                    log_info(logger, "PID: <%u> - CHECK INTERRUPT (Negado) - el proceso se fue de CPU", pcb->pid);
                    puede_seguir_ejecutando = false;

                    desalojo = false;
                }
            }
            else{
                log_info(logger, "PID: <%u> - CHECK INTERRUPT (False)", pcb->pid);
            }
        sem_post(&mutex_desalojo);

        destruir_elemento_instruccion((void*) instruccion);
        instruccion = NULL;

    } while (puede_seguir_ejecutando);

}

void establecer_tiempo_restante_de_ejecucion(t_PCB* pcb,struct timeval inicio, struct timeval final){
    gettimeofday(&final, NULL);
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
        pcb->quantum -= (uint32_t)tiempo_ejecucion_ms;
    }
}

void incrementar_program_counter(t_PCB* pcb, int en_cuanto){
    pcb->program_counter += en_cuanto;
    pcb->registros->PC += en_cuanto;
}

