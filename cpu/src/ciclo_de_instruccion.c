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

                t_list* direcciones_fisicas = list_create();
                size_t bytes = obtener_tamano_registro(pcb, registro_datos);
                
                int direccion_logica = (int)get_registro(pcb, obtener_registro_por_nombre(registro_direccion));

                int estado = MMU(direccion_logica, tamanio_pagina_memoria, pcb->pid, (uint32_t)bytes, direcciones_fisicas);

                if(estado == MMU_OK){
                    void* data_leida = NULL;
                    gestionar_lectura_multipagina(conexion_memoria, direcciones_fisicas, pcb->pid, &data_leida, bytes, logger);

                    // almaceno en registro de datos lo traido de memoria
                    if(data_leida == NULL){
                        log_warning(logger,"data_leida es NULL");
                    }
                    uint32_t valor_entero = cadena_a_valor_entero(data_leida, bytes);
                    log_info(logger, "PID: <%u> - valor Entero Final (%u) de (%u) bytes", pcb->pid, valor_entero, (uint32_t)bytes);
                    set_registro(pcb, valor_entero, obtener_registro_por_nombre(registro_datos));

                    log_info(logger, "PID: <%u> - Finalizando: <MOV_IN> - (%s = %u)", pcb->pid, registro_datos, valor_entero);

                    free(data_leida);
                }

                if(estado == SEGMENTATION_FAULT){
                    log_info(logger, "PID: <%u> - ERROR (SEGMENTATION FAULT): <MOV_IN> - <%s> - <%s>", pcb->pid, registro_datos, registro_direccion);
                    
                    finalizar_pcb_motivo_salida(pcb, inicio, final, conexion, SALIDA_SEGMENTATION_FAULT, &puede_seguir_ejecutando, &proceso_sigue_en_cpu);

                    log_info(logger, "PID: <%u> - Finalizando: <MOV_IN> - <%s> - <%s>", pcb->pid, registro_datos, registro_direccion);
                }

                liberar_lista_de_peticiones_memoria(direcciones_fisicas);
                break;
            }
            case MOV_OUT:
            {
                char* registro_direccion = (char*) list_get(instruccion->parametros, 0);
                char* registro_datos = (char*) list_get(instruccion->parametros, 1);

                log_info(logger, "PID: <%u> - Ejecutando: <MOV_OUT> - <%s> - <%s>", pcb->pid, registro_direccion, registro_datos);
                
                t_list* direcciones_fisicas = list_create();
                size_t bytes = obtener_tamano_registro(pcb, registro_datos);
                int direccion_logica = (int)get_registro(pcb, obtener_registro_por_nombre(registro_direccion));
                
                int estado = MMU(direccion_logica, tamanio_pagina_memoria, pcb->pid, (uint32_t)bytes, direcciones_fisicas);

                if(estado == MMU_OK){
                    uint32_t valor_entero = get_registro(pcb, obtener_registro_por_nombre(registro_datos));
                    log_info(logger, "PID: <%u> - valor Entero a Escribir (%u) de (%u) bytes", pcb->pid, valor_entero, (uint32_t)bytes);

                    char* valor_a_escribir = valor_entero_a_cadena_nueva(valor_entero, bytes); 
                    
                    gestionar_escritura_multipagina(conexion_memoria, direcciones_fisicas, pcb->pid, (void*) valor_a_escribir , bytes, logger);

                    free(valor_a_escribir);

                    log_info(logger, "PID: <%u> - Finalizando: <MOV_OUT> - <%s> - <%s>", pcb->pid, registro_direccion, registro_datos);
                }

                if(estado == SEGMENTATION_FAULT){
                    log_info(logger, "PID: <%u> - ERROR (SEGMENTATION FAULT): <MOV_OUT> - <%s> - <%s>", pcb->pid, registro_direccion, registro_datos);
                    
                    finalizar_pcb_motivo_salida(pcb, inicio, final, conexion, SALIDA_SEGMENTATION_FAULT, &puede_seguir_ejecutando, &proceso_sigue_en_cpu);

                    log_info(logger, "PID: <%u> - Finalizando: <MOV_OUT> - <%s> - <%s>", pcb->pid, registro_direccion, registro_datos);
                }

                liberar_lista_de_peticiones_memoria(direcciones_fisicas);

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
            case RESIZE:
            {
                int tamanio = atoi((char*) list_get(instruccion->parametros, 0));

                log_info(logger, "PID: <%u> - Ejecutando: <RESIZE> - <%d>", pcb->pid, tamanio);

                enviar_generico_doble_entero(conexion_memoria, SOLICITUD_RESIZE_MEMORIA, pcb->pid,(uint32_t)tamanio);

                int respuesta = recibo_generico_op_code(conexion_memoria);

                if(respuesta == MEMORIA_OK){
                    log_info(logger, "PID: <%u> - Finalizando: <RESIZE> - <%d>", pcb->pid, tamanio);
                }

                if(respuesta == OUT_OF_MEMORY){
                    log_info(logger, "PID: <%u> - ERROR (OUT_OF_MEMORY): <RESIZE> - <%d>", pcb->pid, tamanio);
                    
                    finalizar_pcb_motivo_salida(pcb, inicio, final, conexion, SALIDA_OUT_OF_MEMORY, &puede_seguir_ejecutando, &proceso_sigue_en_cpu);
                    
                    log_info(logger, "PID: <%u> - Finalizando: <RESIZE> - <%d>", pcb->pid, tamanio);
                }

                break;
            }
            case COPY_STRING:
            {
                int tamanio = atoi((char*) list_get(instruccion->parametros, 0));

                log_info(logger, "PID: <%u> - Ejecutando: <COPY_STRING> - <(tamanio = %d)>", pcb->pid, tamanio);

                int direccion_logica_SI = (int)get_registro(pcb, obtener_registro_por_nombre("SI"));
                log_info(logger, "PID: <%u> - Ejecutando: <COPY_STRING> - <(SI = %d)>", pcb->pid, direccion_logica_SI);
                int direccion_logica_DI = (int)get_registro(pcb, obtener_registro_por_nombre("DI"));
                log_info(logger, "PID: <%u> - Ejecutando: <COPY_STRING> - <(DI = %d)>", pcb->pid, direccion_logica_DI);

                t_list* direcciones_fisicas_read = list_create();
                int estado_r = MMU(direccion_logica_SI, tamanio_pagina_memoria, pcb->pid, (uint32_t)tamanio, direcciones_fisicas_read);
                
                // variable donde se almacenara lo leido para luego ser copiado
                void* string_final = NULL; 

                if(estado_r == MMU_OK){
                    gestionar_lectura_multipagina(conexion_memoria, direcciones_fisicas_read, pcb->pid, &string_final, (uint32_t)tamanio, logger);
                    
                    char* string_leido = convertir_a_cadena_nueva(string_final, (size_t)tamanio);
                    log_info(logger, "PID: <%u> - Ejecutando: <COPY_STRING> - String Leido (%s)", pcb->pid, string_leido);
                    free(string_leido);

                    liberar_lista_de_peticiones_memoria(direcciones_fisicas_read);
                }

                if(estado_r == SEGMENTATION_FAULT){
                    liberar_lista_de_peticiones_memoria(direcciones_fisicas_read);

                    log_info(logger, "PID: <%u> - ERROR LECTURA (SEGMENTATION FAULT): <COPY_STRING> ", pcb->pid);
                    
                    finalizar_pcb_motivo_salida(pcb, inicio, final, conexion, SALIDA_SEGMENTATION_FAULT, &puede_seguir_ejecutando, &proceso_sigue_en_cpu);
                    
                    log_info(logger, "PID: <%u> - Finalizando: <COPY_STRING> ", pcb->pid);

                    break; // para no pasar a la soguiente traduccion (par escritura)
                }

                t_list* direcciones_fisicas_write = list_create();
                int estado_w = MMU(direccion_logica_DI, tamanio_pagina_memoria, pcb->pid, (uint32_t)tamanio, direcciones_fisicas_write);

                if(estado_w == MMU_OK){
                    gestionar_escritura_multipagina(conexion_memoria, direcciones_fisicas_write, pcb->pid, string_final, (uint32_t)tamanio, logger);

                    log_info(logger, "PID: <%u> - Finalizando: <COPY_STRING> ", pcb->pid);
                }

                if(estado_w == SEGMENTATION_FAULT){
                    log_info(logger, "PID: <%u> - ERROR ESCRITURA (SEGMENTATION FAULT): <COPY_STRING>", pcb->pid);
                    
                    finalizar_pcb_motivo_salida(pcb, inicio, final, conexion, SALIDA_SEGMENTATION_FAULT, &puede_seguir_ejecutando, &proceso_sigue_en_cpu);
                    
                    log_info(logger, "PID: <%u> - Finalizando: <COPY_STRING>", pcb->pid);
                }

                free(string_final); // liberamos
                liberar_lista_de_peticiones_memoria(direcciones_fisicas_write);

                break;
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
                enviar_generico_string(conexion, IGNORAR_OP_CODE, nombre_recurso);
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
                enviar_generico_string(conexion, IGNORAR_OP_CODE, nombre_recurso);
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
                enviar_pcb(conexion, pcb, PETICION_IO); // tipo solicitud
                enviar_generico_string(conexion, GENERICA, nombre_interfaz); // tipo y nombre interfaz
                enviar_generico_entero(conexion, IO_GEN_SLEEP, (uint32_t)unidades_de_trabajo); // tipo operacion y parametros
                puede_seguir_ejecutando = false;
                proceso_sigue_en_cpu = false;

                log_info(logger, "PID: <%u> - Finalizando: <IO_GEN_SLEEP> ", pcb->pid);
                break;
            }
            case IO_STDIN_READ:
            {
                char* nombre_interfaz = (char*) list_get(instruccion->parametros, 0);
                char* registro_direccion = (char*) list_get(instruccion->parametros, 1);
                char* registro_tamanio = (char*) list_get(instruccion->parametros, 2);

                int valor_registro_direccion = (int)get_registro(pcb, obtener_registro_por_nombre(registro_direccion));
                int tamanio = (int)get_registro(pcb, obtener_registro_por_nombre(registro_tamanio)); // en bytes supongo

                log_info(logger, "PID: <%u> - Ejecutando: <IO_STDIN_READ> - <%s> - <(%s = %d)> - <(%s = %d)>", pcb->pid, nombre_interfaz, registro_direccion, valor_registro_direccion, registro_tamanio, tamanio);
                
                t_list* direcciones_fisicas = list_create();
                int estado = MMU(valor_registro_direccion, tamanio_pagina_memoria, pcb->pid, (uint32_t)tamanio, direcciones_fisicas);

                if(estado == MMU_OK){
                    // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
                    incrementar_program_counter(pcb, 1);

                    establecer_tiempo_restante_de_ejecucion(pcb, inicio, final); 

                    log_info(logger, "PID: <%u> - Se va de CPU", pcb->pid);
                    enviar_pcb(conexion, pcb, PETICION_IO); // tipo solicitud
                    enviar_generico_string(conexion, STDIN, nombre_interfaz); // tipo y nombre interfaz
                    enviar_generico_entero(conexion, IO_STDIN_READ, (uint32_t) tamanio); // tipo operacion y parametros
                    enviar_lista_peticiones_memoria(conexion, IGNORAR_OP_CODE, direcciones_fisicas); // mas parametros
                    puede_seguir_ejecutando = false;
                    proceso_sigue_en_cpu = false;

                    log_info(logger, "PID: <%u> - Finalizando: <IO_STDIN_READ> ", pcb->pid);
                }

                if(estado == SEGMENTATION_FAULT){
                    log_info(logger, "PID: <%u> - ERROR ESCRITURA (SEGMENTATION FAULT): <IO_STDIN_READ> ", pcb->pid);

                    finalizar_pcb_motivo_salida(pcb, inicio, final, conexion, SALIDA_SEGMENTATION_FAULT, &puede_seguir_ejecutando, &proceso_sigue_en_cpu);

                    log_info(logger, "PID: <%u> - Finalizando: <IO_STDIN_READ> ", pcb->pid);
                }

                liberar_lista_de_peticiones_memoria(direcciones_fisicas);

                break;
            }
            case IO_STDOUT_WRITE:
            {
                char* nombre_interfaz = (char*) list_get(instruccion->parametros, 0);
                char* registro_direccion = (char*) list_get(instruccion->parametros, 1);
                char* registro_tamanio = (char*) list_get(instruccion->parametros, 2);

                int valor_registro_direccion = (int)get_registro(pcb, obtener_registro_por_nombre(registro_direccion));
                int tamanio = (int)get_registro(pcb, obtener_registro_por_nombre(registro_tamanio)); // en bytes supongo

                log_info(logger, "PID: <%u> - Ejecutando: <IO_STDOUT_WRITE> - <%s> - <(%s = %d)> - <(%s = %d)>", pcb->pid, nombre_interfaz, registro_direccion, valor_registro_direccion, registro_tamanio, tamanio);
                
                t_list* direcciones_fisicas = list_create();
                int estado = MMU(valor_registro_direccion, tamanio_pagina_memoria, pcb->pid, (uint32_t)tamanio, direcciones_fisicas);

                if(estado == MMU_OK){
                    // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
                    incrementar_program_counter(pcb, 1);

                    establecer_tiempo_restante_de_ejecucion(pcb, inicio, final); 

                    log_info(logger, "PID: <%u> - Se va de CPU", pcb->pid);
                    enviar_pcb(conexion, pcb, PETICION_IO); // tipo solicitud
                    enviar_generico_string(conexion, STDOUT, nombre_interfaz); // tipo y nombre interfaz
                    enviar_generico_entero(conexion, IO_STDOUT_WRITE, (uint32_t) tamanio); // tipo operacion y parametros
                    enviar_lista_peticiones_memoria(conexion, IGNORAR_OP_CODE, direcciones_fisicas); // mas parametros
                    puede_seguir_ejecutando = false;
                    proceso_sigue_en_cpu = false;

                    log_info(logger, "PID: <%u> - Finalizando: <IO_STDOUT_WRITE> ", pcb->pid);
                }

                if(estado == SEGMENTATION_FAULT){
                    log_info(logger, "PID: <%u> - ERROR ESCRITURA (SEGMENTATION FAULT): <IO_STDOUT_WRITE> ", pcb->pid);

                    finalizar_pcb_motivo_salida(pcb, inicio, final, conexion, SALIDA_SEGMENTATION_FAULT, &puede_seguir_ejecutando, &proceso_sigue_en_cpu);

                    log_info(logger, "PID: <%u> - Finalizando: <IO_STDOUT_WRITE> ", pcb->pid);
                }

                liberar_lista_de_peticiones_memoria(direcciones_fisicas);

                break;
            }
            case IO_FS_CREATE:
            {
                char* nombre_interfaz = (char*) list_get(instruccion->parametros, 0);
                char* nombre_archivo = (char*) list_get(instruccion->parametros, 1);

                log_info(logger, "PID: <%u> - Ejecutando: <IO_FS_CREATE> - <%s> - <%s>", pcb->pid, nombre_interfaz, nombre_archivo);
                
                // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
                incrementar_program_counter(pcb, 1);

                establecer_tiempo_restante_de_ejecucion(pcb, inicio, final); 

                log_info(logger, "PID: <%u> - Se va de CPU", pcb->pid);
                enviar_pcb(conexion, pcb, PETICION_IO); // tipo solicitud
                enviar_generico_string(conexion, DIALFS, nombre_interfaz); // tipo y nombre interfaz
                enviar_generico_string(conexion, IO_FS_CREATE, nombre_archivo); // tipo operacion y parametros
                puede_seguir_ejecutando = false;
                proceso_sigue_en_cpu = false;

                log_info(logger, "PID: <%u> - Finalizando: <IO_FS_CREATE>", pcb->pid);
                break;
            }
            case IO_FS_DELETE:
            {
                char* nombre_interfaz = (char*) list_get(instruccion->parametros, 0);
                char* nombre_archivo = (char*) list_get(instruccion->parametros, 1);

                log_info(logger, "PID: <%u> - Ejecutando: <IO_FS_DELETE> - <%s> - <%s>", pcb->pid, nombre_interfaz, nombre_archivo);
                
                // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
                incrementar_program_counter(pcb, 1);

                establecer_tiempo_restante_de_ejecucion(pcb, inicio, final); 

                log_info(logger, "PID: <%u> - Se va de CPU", pcb->pid);
                enviar_pcb(conexion, pcb, PETICION_IO); // tipo solicitud
                enviar_generico_string(conexion, DIALFS, nombre_interfaz); // tipo y nombre interfaz
                enviar_generico_string(conexion, IO_FS_DELETE, nombre_archivo); // tipo operacion y parametros
                puede_seguir_ejecutando = false;
                proceso_sigue_en_cpu = false;

                log_info(logger, "PID: <%u> - Finalizando: <IO_FS_DELETE>", pcb->pid);
                break;
            }
            case IO_FS_TRUNCATE:
            {
                char* nombre_interfaz = (char*) list_get(instruccion->parametros, 0);
                char* nombre_archivo = (char*) list_get(instruccion->parametros, 1);
                char* registro_tamanio = (char*) list_get(instruccion->parametros, 2);

                int tamanio_bytes = (int)get_registro(pcb, obtener_registro_por_nombre(registro_tamanio)); 

                log_info(logger, "PID: <%u> - Ejecutando: <IO_FS_TRUNCATE> - <%s> - <%s> - <(%s = %d)>", pcb->pid, nombre_interfaz, nombre_archivo, registro_tamanio, tamanio_bytes);
                
                // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
                incrementar_program_counter(pcb, 1);

                establecer_tiempo_restante_de_ejecucion(pcb, inicio, final); 

                log_info(logger, "PID: <%u> - Se va de CPU", pcb->pid);
                enviar_pcb(conexion, pcb, PETICION_IO); // tipo solicitud
                enviar_generico_string(conexion, DIALFS, nombre_interfaz); // tipo y nombre interfaz
                envio_generico_entero_y_string(conexion, IO_FS_TRUNCATE, (uint32_t) tamanio_bytes, nombre_archivo); // tipo operacion y parametros
                puede_seguir_ejecutando = false;
                proceso_sigue_en_cpu = false;

                log_info(logger, "PID: <%u> - Finalizando: <IO_FS_TRUNCATE>", pcb->pid);
                break;
            }
            case IO_FS_WRITE:
            {
                char* nombre_interfaz = (char*) list_get(instruccion->parametros, 0);
                char* nombre_archivo = (char*) list_get(instruccion->parametros, 1);
                char* registro_direccion = (char*) list_get(instruccion->parametros, 2);
                char* registro_tamanio = (char*) list_get(instruccion->parametros, 3);
                char* registro_puntero = (char*) list_get(instruccion->parametros, 4);

                int valor_registro_direccion = (int)get_registro(pcb, obtener_registro_por_nombre(registro_direccion));
                int tamanio_bytes = (int)get_registro(pcb, obtener_registro_por_nombre(registro_tamanio)); 
                int puntero = (int)get_registro(pcb, obtener_registro_por_nombre(registro_puntero)); 

                log_info(logger, "PID: <%u> - Ejecutando: <IO_FS_WRITE> - <%s> - <%s> - <(%s = %d)> - <(%s = %d)> - <(%s = %d)>",
                pcb->pid, nombre_interfaz, nombre_archivo, registro_direccion, valor_registro_direccion, registro_tamanio, tamanio_bytes, registro_puntero, puntero);
                
                t_list* direcciones_fisicas = list_create();
                int estado = MMU(valor_registro_direccion, tamanio_pagina_memoria, pcb->pid, (uint32_t)tamanio_bytes, direcciones_fisicas);

                if(estado == MMU_OK){
                    // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
                    incrementar_program_counter(pcb, 1);

                    establecer_tiempo_restante_de_ejecucion(pcb, inicio, final); 

                    log_info(logger, "PID: <%u> - Se va de CPU", pcb->pid);
                    enviar_pcb(conexion, pcb, PETICION_IO); // tipo solicitud
                    enviar_generico_string(conexion, DIALFS, nombre_interfaz); // tipo y nombre interfaz
                    enviar_generico_doble_entero_y_string(conexion, IO_FS_WRITE, (uint32_t)tamanio_bytes, (uint32_t)puntero, nombre_archivo); // tipo operacion y parametros
                    enviar_lista_peticiones_memoria(conexion, IGNORAR_OP_CODE, direcciones_fisicas); // mas parametros
                    puede_seguir_ejecutando = false;
                    proceso_sigue_en_cpu = false;

                    log_info(logger, "PID: <%u> - Finalizando: <IO_FS_WRITE> ", pcb->pid);
                }

                if(estado == SEGMENTATION_FAULT){
                    log_info(logger, "PID: <%u> - ERROR ESCRITURA (SEGMENTATION FAULT): <IO_FS_WRITE> ", pcb->pid);

                    finalizar_pcb_motivo_salida(pcb, inicio, final, conexion, SALIDA_SEGMENTATION_FAULT, &puede_seguir_ejecutando, &proceso_sigue_en_cpu);

                    log_info(logger, "PID: <%u> - Finalizando: <IO_FS_WRITE> ", pcb->pid);
                }

                liberar_lista_de_peticiones_memoria(direcciones_fisicas);

                break;
            }
            case IO_FS_READ:
            {
                char* nombre_interfaz = (char*) list_get(instruccion->parametros, 0);
                char* nombre_archivo = (char*) list_get(instruccion->parametros, 1);
                char* registro_direccion = (char*) list_get(instruccion->parametros, 2);
                char* registro_tamanio = (char*) list_get(instruccion->parametros, 3);
                char* registro_puntero = (char*) list_get(instruccion->parametros, 4);

                int valor_registro_direccion = (int)get_registro(pcb, obtener_registro_por_nombre(registro_direccion));
                int tamanio_bytes = (int)get_registro(pcb, obtener_registro_por_nombre(registro_tamanio)); 
                int puntero = (int)get_registro(pcb, obtener_registro_por_nombre(registro_puntero)); 

                log_info(logger, "PID: <%u> - Ejecutando: <IO_FS_READ> - <%s> - <%s> - <(%s = %d)> - <(%s = %d)> - <(%s = %d)>",
                pcb->pid, nombre_interfaz, nombre_archivo, registro_direccion, valor_registro_direccion, registro_tamanio, tamanio_bytes, registro_puntero, puntero);
                
                t_list* direcciones_fisicas = list_create();
                int estado = MMU(valor_registro_direccion, tamanio_pagina_memoria, pcb->pid, (uint32_t)tamanio_bytes, direcciones_fisicas);

                if(estado == MMU_OK){
                    // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
                    incrementar_program_counter(pcb, 1);

                    establecer_tiempo_restante_de_ejecucion(pcb, inicio, final); 

                    log_info(logger, "PID: <%u> - Se va de CPU", pcb->pid);
                    enviar_pcb(conexion, pcb, PETICION_IO); // tipo solicitud
                    enviar_generico_string(conexion, DIALFS, nombre_interfaz); // tipo y nombre interfaz
                    enviar_generico_doble_entero_y_string(conexion, IO_FS_READ, (uint32_t)tamanio_bytes, (uint32_t)puntero, nombre_archivo); // tipo operacion y parametros
                    enviar_lista_peticiones_memoria(conexion, IGNORAR_OP_CODE, direcciones_fisicas); // mas parametros
                    puede_seguir_ejecutando = false;
                    proceso_sigue_en_cpu = false;

                    log_info(logger, "PID: <%u> - Finalizando: <IO_FS_READ> ", pcb->pid);
                }

                if(estado == SEGMENTATION_FAULT){
                    log_info(logger, "PID: <%u> - ERROR ESCRITURA (SEGMENTATION FAULT): <IO_FS_READ> ", pcb->pid);

                    finalizar_pcb_motivo_salida(pcb, inicio, final, conexion, SALIDA_SEGMENTATION_FAULT, &puede_seguir_ejecutando, &proceso_sigue_en_cpu);

                    log_info(logger, "PID: <%u> - Finalizando: <IO_FS_READ> ", pcb->pid);
                }

                liberar_lista_de_peticiones_memoria(direcciones_fisicas);

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
                envio_generico_op_code(conexion, SALIDA_SUCCESS);
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

void finalizar_pcb_motivo_salida(t_PCB* pcb, struct timeval inicio, struct timeval final, int conexion, motivo_exit motivo, bool* puede_seguir_ejecutando, bool* proceso_sigue_en_cpu){
    // porque el pcb se va de cpu, si no lo hacemos tendra el PC desactualizado
    incrementar_program_counter(pcb, 1);

    establecer_tiempo_restante_de_ejecucion(pcb, inicio, final); 

    log_info(logger, "PID: <%u> - Se va de CPU", pcb->pid);
    enviar_pcb(conexion, pcb, PROCESO_FINALIZADO);
    // ejemplo envio_generico_op_code(conexion, SALIDA_SEGMENTATION_FAULT);
    envio_generico_op_code(conexion, motivo);
    *puede_seguir_ejecutando = false;
    *proceso_sigue_en_cpu = false;
}