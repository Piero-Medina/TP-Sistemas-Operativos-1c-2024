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

        // MOV_IN, MOV_OUT, RESIZE, COPY_STRING, IO_STDIN_READ,IO_STDOUT_WRITE
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
            case MOV_IN:
            {
                char* registro_datos = (char*) list_get(instruccion->parametros, 0);
                char* registro_direccion = (char*) list_get(instruccion->parametros, 1);

                log_info(logger, "PID: <%u> - Ejecutando: <MOV_IN> - <%s> - <%s>", pcb->pid, registro_datos, registro_direccion);

                uint32_t direccion_fisica;
                int estado = mmu((int)get_registro(pcb, obtener_registro_por_nombre(registro_direccion)), tamanio_pagina_memoria, pcb->pid, &direccion_fisica);

                if(estado == MMU_OK){
                    size_t bytes = obtener_tamano_registro(pcb, registro_datos);
                    envio_generico_doble_entero(conexion_memoria, SOLICITUD_LECTURA_MEMORIA, direccion_fisica, (uint32_t)bytes);

                    // recibo algo de memoria
                    ignorar_op_code(conexion_memoria);
                    void* data = recibir_data(conexion_memoria, NULL);

                    char* data_leida = convertir_a_cadena(data, bytes);
                    log_info(logger, "PID: <%u> - Acción: <LEER> - Dirección Física: <%u> - Valor: <%s>", pcb->pid, direccion_fisica, data_leida);

                    // almaceno en registro de datos lo traido de memoria
                    uint32_t valor_entero = cadena_a_valor_entero((void*)data_leida, bytes);
                    set_registro(pcb, valor_entero, obtener_registro_por_nombre(registro_datos));
                    
                    // asumiendo que realloc tuvo exito
                    free(data_leida);

                    log_info(logger, "PID: <%u> - Finalizando: <MOV_IN> - (%s = %u)", pcb->pid, registro_datos, valor_entero);
                }

                if(estado == SEGMENTATION_FAULT){

                    log_info(logger, "PID: <%u> - ERROR (SEGMENTATION FAULT): <MOV_IN> - <%s> - <%s>", pcb->pid, registro_datos, registro_direccion);
                    
                    // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
                    incrementar_program_counter(pcb, 1);

                    establecer_tiempo_restante_de_ejecucion(pcb, inicio, final); 

                    log_info(logger, "PID: <%u> - Se va de CPU", pcb->pid);
                    enviar_pcb(conexion, pcb, SEGMENTATION_FAULT);
                    puede_seguir_ejecutando = false;
                    proceso_sigue_en_cpu = false;

                    log_info(logger, "PID: <%u> - Finalizando: <MOV_IN> - <%s> - <%s>", pcb->pid, registro_datos, registro_direccion);
                }

                break;
            }
            case MOV_OUT:
            {
                //TODO
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
            case RESIZE:
            {
                //TODO
            }
            case COPY_STRING:
            {
                //TODO
            }
            case WAIT:
            {
                char* nombre_recurso = (char*) list_get(instruccion->parametros, 0);

                log_info(logger, "PID: <%u> - Ejecutando: <WAIT> - <%s>", pcb->pid, nombre_recurso);
                
                // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
                incrementar_program_counter(pcb, 1);

                establecer_tiempo_restante_de_ejecucion(pcb, inicio, final); 

                log_info(logger, "PID: <%u> - Se va de CPU", pcb->pid);
                enviar_pcb(conexion, pcb, WAIT_KERNEL);
                envio_generico_string(conexion, IGNORAR_OP_CODE, nombre_recurso);
                puede_seguir_ejecutando = false;
                proceso_sigue_en_cpu = false;

                log_info(logger, "PID: <%u> - Finalizando: <WAIT> - <%s>", pcb->pid, nombre_recurso);
                break;
            }
            case SIGNAL:
            {
                char* nombre_recurso = (char*) list_get(instruccion->parametros, 0);

                log_info(logger, "PID: <%u> - Ejecutando: <SIGNAL> - <%s>", pcb->pid, nombre_recurso);
                
                // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
                incrementar_program_counter(pcb, 1);

                establecer_tiempo_restante_de_ejecucion(pcb, inicio, final); 

                log_info(logger, "PID: <%u> - Se va de CPU", pcb->pid);
                enviar_pcb(conexion, pcb, SIGNAL_KERNEL);
                envio_generico_string(conexion, IGNORAR_OP_CODE, nombre_recurso);
                puede_seguir_ejecutando = false;
                proceso_sigue_en_cpu = false;

                log_info(logger, "PID: <%u> - Finalizando: <SIGNAL> - <%s>", pcb->pid, nombre_recurso);
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
            case IO_STDIN_READ:
            {
                //TODO
            }
            case IO_STDOUT_WRITE:
            {
                //TODO
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

void establecer_tiempo_restante_de_ejecucion(t_PCB* pcb, struct timeval inicio, struct timeval final){
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

