#include "manejo_interfaz.h"


void agregar_interfaz(t_dictionary* interfaces, char* nombre_interfaz, int conexion, int tipo_interfaz){
    t_interfaz* tmp = malloc(sizeof(t_interfaz));
    
    tmp->socket = conexion;
    tmp->tipo = tipo_interfaz;
    tmp->ocupado = false;
    sem_init(&tmp->semaforo, 0, 1);
    tmp->cola = queue_create();

    dictionary_put(interfaces, nombre_interfaz, (void*) tmp);
}

void quitar_interfaz(t_dictionary* interfaces, char* nombre_interfaz){
    t_interfaz* interfaz = NULL;
    t_io_pendiente* io_pendiente = NULL;
    t_PCB* pcb = NULL;

    interfaz = (t_interfaz*)dictionary_get(interfaces, nombre_interfaz);

    // si la cola no esta vacia, entonces habia procesos en blocked esperando a procesar IO.
    if(!queue_is_empty(interfaz->cola)){
        for(int i = 0; i < queue_size(interfaz->cola); i++){
            io_pendiente = list_get(interfaz->cola->elements, i);
            
            if(io_pendiente != NULL){
                sem_wait(&mutex_cola_blocked);
                pcb = buscar_pcb_por_pid_y_remover(io_pendiente->pid, cola_blocked->elements);
                sem_wait(&mutex_cola_blocked);

                mandar_a_exit(pcb, "INTERFAZ_DESCONECTADA");
                sem_post(&sem_grado_multiprogramacion);

                sem_wait(&sem_peticiones_io_por_procesar);
            }
        }
    }

    // nos fijamos si alguna proceso estaba ejecutando esta IO en especifico
    // si lo estaba, este no volvera, por lo que mandamos a exit
    for(int i = 0; i < list_size(io_pendientes_ejecutando); i++){
        io_pendiente = list_get(io_pendientes_ejecutando, i);
            
        if((strcmp(io_pendiente->nombre_interfaz, nombre_interfaz) == 0)){
            sem_wait(&mutex_cola_blocked);
                pcb = buscar_pcb_por_pid_y_remover(io_pendiente->pid, cola_blocked->elements);
            sem_wait(&mutex_cola_blocked);

            mandar_a_exit(pcb, "INTERFAZ_DESCONECTADA");
            sem_post(&sem_grado_multiprogramacion);

            //sem_wait(&sem_peticiones_io_por_procesar); No porque ya se consumio al estar en ejecucion
        }
    }

    dictionary_remove_and_destroy(interfaces, nombre_interfaz, liberar_elemento_interfaz);
}

//
void verificar_tipo_interfaz(int conexion, t_PCB* pcb){
    int tipo_de_interfaz = recibo_generico_op_code(conexion_cpu_dispatch);
    char* nombre_interfaz = recibir_generico_string(conexion_cpu_dispatch);

    switch (tipo_de_interfaz){
        case GENERICA:
            verificar_operacion_generica(conexion, nombre_interfaz, pcb);
            free(nombre_interfaz);
            break;
        case STDIN:
            verificar_operacion_stdin(conexion, nombre_interfaz, pcb);
            free(nombre_interfaz);
            break;
        case STDOUT:
            verificar_operacion_stdout(conexion, nombre_interfaz, pcb);
            free(nombre_interfaz);
            break;
        case DIALFS:
            verificar_operacion_dialFs(conexion, nombre_interfaz, pcb);
            free(nombre_interfaz);
            break;
    }
}

void verificar_operacion_generica(int conexion, char* nombre_interfaz, t_PCB* pcb){
    int operacion = recibo_generico_op_code(conexion);

    t_interfaz* interfaz = NULL;

    if(operacion == IO_GEN_SLEEP){
        uint32_t unidades_genericas = recibir_generico_entero(conexion);
        sem_post(&mutex_conexion_cpu_dispatch); // terminamos de recibir todos los datos

        // si entra, se mando a finalizar al proceso antes de que llegue de CPU
        ////////////////////////////////////////////////////////////////////// 
        if(pendiente_de_finalizacion_fuera_de_kernel(pcb)){
            sem_post(&sem_grado_multiprogramacion); 
            return;
        }
        ////////////////////////////////////////////////////////////////////// 

        // validar que exista el nombre de interfaz
        sem_wait(&mutex_diccionario_interfaces);
        if(!validar_existencia_nombre_interfaz(interfaces, operacion, nombre_interfaz, pcb)){
            sem_post(&mutex_diccionario_interfaces);
            return;
        }
        
        interfaz = (t_interfaz*) dictionary_get(interfaces, nombre_interfaz);

        // validar que admite la operacion
        if(!validar_que_interfaz_admita_operacion(interfaz, operacion, nombre_interfaz, pcb)){
            sem_post(&mutex_diccionario_interfaces);
            return;
        }

        log_info(logger, "PID: <%u> - Bloqueado por: <INTERFAZ (%s)>", pcb->pid, nombre_interfaz);

        // aca dentro se actualiza el contexto de ejecucion
        if(mover_execute_a_blocked(pcb) == false){
           log_debug(logger, "PID: <%u> - interceptado antes de bloquearse", pcb->pid);
           return; // salimos de la funcion
        }

        // param_1 = unidades_genericas 
        t_io_pendiente* pendiente_de_io = NULL;
        pendiente_de_io = inicializar_io_pendiente(pcb->pid, nombre_interfaz, operacion, false, NULL, NULL, unidades_genericas, 0, 0, 0); //
            
        queue_push(interfaz->cola, (void*) pendiente_de_io);
        sem_post(&mutex_diccionario_interfaces);

        sem_post(&sem_peticiones_io_por_procesar);
    }
}

void verificar_operacion_stdin(int conexion, char* nombre_interfaz, t_PCB* pcb){
    int operacion = recibo_generico_op_code(conexion); 

    t_interfaz* interfaz = NULL;

    if(operacion == IO_STDIN_READ){
        uint32_t tamanio_bytes = recibir_generico_entero(conexion); //
        ignorar_op_code(conexion);
        t_list* direcciones = recibir_lista_peticiones_memoria(conexion); //
        sem_post(&mutex_conexion_cpu_dispatch); // terminamos de recibir todos los datos

        // si entra, se mando a finalizar al proceso antes de que llegue de CPU
        ////////////////////////////////////////////////////////////////////// 
        if(pendiente_de_finalizacion_fuera_de_kernel(pcb)){
            sem_post(&sem_grado_multiprogramacion);
            liberar_lista_de_peticiones_memoria(direcciones); 
            return;
        }
        ////////////////////////////////////////////////////////////////////// 

        // validar que exista el nombre de interfaz
        sem_wait(&mutex_diccionario_interfaces);
        if(!validar_existencia_nombre_interfaz(interfaces, operacion, nombre_interfaz, pcb)){
            sem_post(&mutex_diccionario_interfaces);
            liberar_lista_de_peticiones_memoria(direcciones); 
            return;
        }
        
        interfaz = (t_interfaz*) dictionary_get(interfaces, nombre_interfaz);

        // validar que admite la operacion
        if(!validar_que_interfaz_admita_operacion(interfaz, operacion, nombre_interfaz, pcb)){
            sem_post(&mutex_diccionario_interfaces);
            liberar_lista_de_peticiones_memoria(direcciones); 
            return;
        }

        log_info(logger, "PID: <%u> - Bloqueado por: <INTERFAZ (%s)>", pcb->pid, nombre_interfaz);

        // aca dentro se actualiza el contexto de ejecucion
        if(mover_execute_a_blocked(pcb) == false){
           log_debug(logger, "PID: <%u> - interceptado antes de bloquearse", pcb->pid);
           liberar_lista_de_peticiones_memoria(direcciones); 
           return; // salimos de la funcion
        }

        // param_1 = tamanio_bytes | peticiones = direcciones
        t_io_pendiente* pendiente_de_io = NULL;
        pendiente_de_io = inicializar_io_pendiente(pcb->pid, nombre_interfaz, operacion, false, NULL, direcciones, tamanio_bytes, 0, 0, 0); //
            
        queue_push(interfaz->cola, (void*) pendiente_de_io);
        sem_post(&mutex_diccionario_interfaces);

        sem_post(&sem_peticiones_io_por_procesar);

    }
}

void verificar_operacion_stdout(int conexion, char* nombre_interfaz, t_PCB* pcb){
    int operacion = recibo_generico_op_code(conexion); 

    t_interfaz* interfaz = NULL;

    if(operacion == IO_STDOUT_WRITE){
        uint32_t tamanio_bytes = recibir_generico_entero(conexion); //
        ignorar_op_code(conexion);
        t_list* direcciones = recibir_lista_peticiones_memoria(conexion); //
        sem_post(&mutex_conexion_cpu_dispatch); // terminamos de recibir todos los datos

        //log_warning(logger, "Imprimimos peticiones Recien llegadas");
        //printf("Imprimimos peticiones Recien llegadas \n");
        //imprimir_lista_peticion_memoria(direcciones);

        // si entra, se mando a finalizar al proceso antes de que llegue de CPU
        ////////////////////////////////////////////////////////////////////// 
        if(pendiente_de_finalizacion_fuera_de_kernel(pcb)){
            sem_post(&sem_grado_multiprogramacion);
            liberar_lista_de_peticiones_memoria(direcciones);  
            return;
        }
        ////////////////////////////////////////////////////////////////////// 

        // validar que exista el nombre de interfaz
        sem_wait(&mutex_diccionario_interfaces);
        if(!validar_existencia_nombre_interfaz(interfaces, operacion, nombre_interfaz, pcb)){
            sem_post(&mutex_diccionario_interfaces);
            liberar_lista_de_peticiones_memoria(direcciones); 
            return;
        }
        
        interfaz = (t_interfaz*) dictionary_get(interfaces, nombre_interfaz);

        // validar que admite la operacion
        if(!validar_que_interfaz_admita_operacion(interfaz, operacion, nombre_interfaz, pcb)){
            sem_post(&mutex_diccionario_interfaces);
            liberar_lista_de_peticiones_memoria(direcciones); 
            return;
        }

        log_info(logger, "PID: <%u> - Bloqueado por: <INTERFAZ (%s)>", pcb->pid, nombre_interfaz);

        // aca dentro se actualiza el contexto de ejecucion
        if(mover_execute_a_blocked(pcb) == false){
           log_debug(logger, "PID: <%u> - interceptado antes de bloquearse", pcb->pid);
           liberar_lista_de_peticiones_memoria(direcciones); 
           return; // salimos de la funcion
        }

        // param_1 = tamanio_bytes | peticiones = direcciones
        t_io_pendiente* pendiente_de_io = NULL;
        pendiente_de_io = inicializar_io_pendiente(pcb->pid, nombre_interfaz, operacion, false, NULL, direcciones, tamanio_bytes, 0, 0, 0); //
            
        queue_push(interfaz->cola, (void*) pendiente_de_io);
        sem_post(&mutex_diccionario_interfaces);

        sem_post(&sem_peticiones_io_por_procesar);

    }
}

void verificar_operacion_dialFs(int conexion, char* nombre_interfaz, t_PCB* pcb){
    int operacion = recibo_generico_op_code(conexion);

    t_interfaz* interfaz = NULL; 

    switch (operacion){
        case IO_FS_CREATE:
        {   
            char* nombre_archivo = recibir_generico_string(conexion);
            sem_post(&mutex_conexion_cpu_dispatch); // terminamos de recibir todos los datos

            // si entra, se mando a finalizar al proceso antes de que llegue de CPU
            ////////////////////////////////////////////////////////////////////// 
            if(pendiente_de_finalizacion_fuera_de_kernel(pcb)){
                sem_post(&sem_grado_multiprogramacion);
                free(nombre_archivo); 
                break;
            }
            ////////////////////////////////////////////////////////////////////// 

            // validar que exista el nombre de interfaz
            sem_wait(&mutex_diccionario_interfaces);
            if(!validar_existencia_nombre_interfaz(interfaces, operacion, nombre_interfaz, pcb)){
                sem_post(&mutex_diccionario_interfaces);
                free(nombre_archivo);
                break;
            }
        
            interfaz = (t_interfaz*) dictionary_get(interfaces, nombre_interfaz);

            // validar que admite la operacion
            if(!validar_que_interfaz_admita_operacion(interfaz, operacion, nombre_interfaz, pcb)){
                sem_post(&mutex_diccionario_interfaces);
                free(nombre_archivo);
                break;
            }

            log_info(logger, "PID: <%u> - Bloqueado por: <INTERFAZ (%s)>", pcb->pid, nombre_interfaz);

            // aca dentro se actualiza el contexto de ejecucion
            if(mover_execute_a_blocked(pcb) == false){
                log_debug(logger, "PID: <%u> - interceptado antes de bloquearse", pcb->pid);
                free(nombre_archivo);
                break; // salimos de la funcion
            }

            // param_string = nombre_archivo
            t_io_pendiente* pendiente_de_io = NULL;
            pendiente_de_io = inicializar_io_pendiente(pcb->pid, nombre_interfaz, operacion, false, nombre_archivo, NULL, 0, 0, 0, 0); //
            
            queue_push(interfaz->cola, (void*) pendiente_de_io);
            sem_post(&mutex_diccionario_interfaces);

            sem_post(&sem_peticiones_io_por_procesar);
            free(nombre_archivo);
            break;
        }
        case IO_FS_DELETE:
        {
            char* nombre_archivo = recibir_generico_string(conexion);
            sem_post(&mutex_conexion_cpu_dispatch); // terminamos de recibir todos los datos

            // si entra, se mando a finalizar al proceso antes de que llegue de CPU
            ////////////////////////////////////////////////////////////////////// 
            if(pendiente_de_finalizacion_fuera_de_kernel(pcb)){
                sem_post(&sem_grado_multiprogramacion);
                free(nombre_archivo); 
                break;
            }
            ////////////////////////////////////////////////////////////////////// 

            // validar que exista el nombre de interfaz
            sem_wait(&mutex_diccionario_interfaces);
            if(!validar_existencia_nombre_interfaz(interfaces, operacion, nombre_interfaz, pcb)){
                sem_post(&mutex_diccionario_interfaces);
                free(nombre_archivo);
                break;
            }
        
            interfaz = (t_interfaz*) dictionary_get(interfaces, nombre_interfaz);

            // validar que admite la operacion
            if(!validar_que_interfaz_admita_operacion(interfaz, operacion, nombre_interfaz, pcb)){
                sem_post(&mutex_diccionario_interfaces);
                free(nombre_archivo);
                break;
            }

            log_info(logger, "PID: <%u> - Bloqueado por: <INTERFAZ (%s)>", pcb->pid, nombre_interfaz);

            // aca dentro se actualiza el contexto de ejecucion
            if(mover_execute_a_blocked(pcb) == false){
                log_debug(logger, "PID: <%u> - interceptado antes de bloquearse", pcb->pid);
                free(nombre_archivo);
                break; // salimos de la funcion
            }

            // param_string = nombre_archivo
            t_io_pendiente* pendiente_de_io = NULL;
            pendiente_de_io = inicializar_io_pendiente(pcb->pid, nombre_interfaz, operacion, false, nombre_archivo, NULL, 0, 0, 0, 0); //
            
            queue_push(interfaz->cola, (void*) pendiente_de_io);
            sem_post(&mutex_diccionario_interfaces);

            sem_post(&sem_peticiones_io_por_procesar);
            free(nombre_archivo);
            break;
        }
        case IO_FS_TRUNCATE:
        {
            char* nombre_archivo;
            uint32_t tamanio_bytes;
            recibir_generico_entero_string(conexion, &tamanio_bytes, &nombre_archivo);
            sem_post(&mutex_conexion_cpu_dispatch); // terminamos de recibir todos los datos

            // si entra, se mando a finalizar al proceso antes de que llegue de CPU
            ////////////////////////////////////////////////////////////////////// 
            if(pendiente_de_finalizacion_fuera_de_kernel(pcb)){
                sem_post(&sem_grado_multiprogramacion);
                free(nombre_archivo); 
                break;
            }
            ////////////////////////////////////////////////////////////////////// 

            // validar que exista el nombre de interfaz
            sem_wait(&mutex_diccionario_interfaces);
            if(!validar_existencia_nombre_interfaz(interfaces, operacion, nombre_interfaz, pcb)){
                sem_post(&mutex_diccionario_interfaces);
                free(nombre_archivo);
                break;
            }
        
            interfaz = (t_interfaz*) dictionary_get(interfaces, nombre_interfaz);

            // validar que admite la operacion
            if(!validar_que_interfaz_admita_operacion(interfaz, operacion, nombre_interfaz, pcb)){
                sem_post(&mutex_diccionario_interfaces);
                free(nombre_archivo);
                break;
            }

            log_info(logger, "PID: <%u> - Bloqueado por: <INTERFAZ (%s)>", pcb->pid, nombre_interfaz);

            // aca dentro se actualiza el contexto de ejecucion
            if(mover_execute_a_blocked(pcb) == false){
                log_debug(logger, "PID: <%u> - interceptado antes de bloquearse", pcb->pid);
                free(nombre_archivo);
                break; // salimos de la funcion
            }

            // param_string = nombre_archivo | param_1 = tamanio_bytes
            t_io_pendiente* pendiente_de_io = NULL;
            pendiente_de_io = inicializar_io_pendiente(pcb->pid, nombre_interfaz, operacion, false, nombre_archivo, NULL, tamanio_bytes, 0, 0, 0); //
            
            queue_push(interfaz->cola, (void*) pendiente_de_io);
            sem_post(&mutex_diccionario_interfaces);

            sem_post(&sem_peticiones_io_por_procesar);
            free(nombre_archivo);
            break;
        }
        case IO_FS_WRITE:
        {
            char* nombre_archivo;
            uint32_t tamanio_bytes, puntero;
            recibir_generico_doble_entero_y_string(conexion, &tamanio_bytes, &puntero, &nombre_archivo);
            ignorar_op_code(conexion);
            t_list* direcciones = recibir_lista_peticiones_memoria(conexion); 
            sem_post(&mutex_conexion_cpu_dispatch); // terminamos de recibir todos los datos

            // si entra, se mando a finalizar al proceso antes de que llegue de CPU
            ////////////////////////////////////////////////////////////////////// 
            if(pendiente_de_finalizacion_fuera_de_kernel(pcb)){
                sem_post(&sem_grado_multiprogramacion);
                free(nombre_archivo); 
                liberar_lista_de_peticiones_memoria(direcciones); 
                break;
            }
            ////////////////////////////////////////////////////////////////////// 

            // validar que exista el nombre de interfaz
            sem_wait(&mutex_diccionario_interfaces);
            if(!validar_existencia_nombre_interfaz(interfaces, operacion, nombre_interfaz, pcb)){
                sem_post(&mutex_diccionario_interfaces);
                free(nombre_archivo);
                liberar_lista_de_peticiones_memoria(direcciones); 
                break;
            }
        
            interfaz = (t_interfaz*) dictionary_get(interfaces, nombre_interfaz);

            // validar que admite la operacion
            if(!validar_que_interfaz_admita_operacion(interfaz, operacion, nombre_interfaz, pcb)){
                sem_post(&mutex_diccionario_interfaces);
                free(nombre_archivo);
                liberar_lista_de_peticiones_memoria(direcciones); 
                break;
            }

            log_info(logger, "PID: <%u> - Bloqueado por: <INTERFAZ (%s)>", pcb->pid, nombre_interfaz);

            // aca dentro se actualiza el contexto de ejecucion
            if(mover_execute_a_blocked(pcb) == false){
                log_debug(logger, "PID: <%u> - interceptado antes de bloquearse", pcb->pid);
                free(nombre_archivo);
                liberar_lista_de_peticiones_memoria(direcciones); 
                break; // salimos de la funcion
            }

            // param_string = nombre_archivo | param_1 = tamanio_bytes | param_2 = puntero | peticiones = direcciones
            t_io_pendiente* pendiente_de_io = NULL;
            pendiente_de_io = inicializar_io_pendiente(pcb->pid, nombre_interfaz, operacion, false, nombre_archivo, direcciones, tamanio_bytes, puntero, 0, 0); //
            
            queue_push(interfaz->cola, (void*) pendiente_de_io);
            sem_post(&mutex_diccionario_interfaces);

            sem_post(&sem_peticiones_io_por_procesar);
            free(nombre_archivo);
            break;
        }
        case IO_FS_READ:
        {
            char* nombre_archivo;
            uint32_t tamanio_bytes, puntero;
            recibir_generico_doble_entero_y_string(conexion, &tamanio_bytes, &puntero, &nombre_archivo);
            ignorar_op_code(conexion);
            t_list* direcciones = recibir_lista_peticiones_memoria(conexion); 
            sem_post(&mutex_conexion_cpu_dispatch); // terminamos de recibir todos los datos

            // si entra, se mando a finalizar al proceso antes de que llegue de CPU
            ////////////////////////////////////////////////////////////////////// 
            if(pendiente_de_finalizacion_fuera_de_kernel(pcb)){
                sem_post(&sem_grado_multiprogramacion);
                free(nombre_archivo);
                liberar_lista_de_peticiones_memoria(direcciones);  
                break;
            }
            ////////////////////////////////////////////////////////////////////// 

            // validar que exista el nombre de interfaz
            sem_wait(&mutex_diccionario_interfaces);
            if(!validar_existencia_nombre_interfaz(interfaces, operacion, nombre_interfaz, pcb)){
                sem_post(&mutex_diccionario_interfaces);
                free(nombre_archivo);
                liberar_lista_de_peticiones_memoria(direcciones); 
                break;
            }
        
            interfaz = (t_interfaz*) dictionary_get(interfaces, nombre_interfaz);

            // validar que admite la operacion
            if(!validar_que_interfaz_admita_operacion(interfaz, operacion, nombre_interfaz, pcb)){
                sem_post(&mutex_diccionario_interfaces);
                free(nombre_archivo);
                liberar_lista_de_peticiones_memoria(direcciones); 
                break;
            }

            log_info(logger, "PID: <%u> - Bloqueado por: <INTERFAZ (%s)>", pcb->pid, nombre_interfaz);

            // aca dentro se actualiza el contexto de ejecucion
            if(mover_execute_a_blocked(pcb) == false){
                log_debug(logger, "PID: <%u> - interceptado antes de bloquearse", pcb->pid);
                free(nombre_archivo);
                liberar_lista_de_peticiones_memoria(direcciones); 
                break; // salimos de la funcion
            }

            // param_string = nombre_archivo | param_1 = tamanio_bytes | param_2 = puntero | peticiones = direcciones
            t_io_pendiente* pendiente_de_io = NULL;
            pendiente_de_io = inicializar_io_pendiente(pcb->pid, nombre_interfaz, operacion, false, nombre_archivo, direcciones, tamanio_bytes, puntero, 0, 0); //
            
            queue_push(interfaz->cola, (void*) pendiente_de_io);
            sem_post(&mutex_diccionario_interfaces);

            sem_post(&sem_peticiones_io_por_procesar);
            free(nombre_archivo);
            break;
        }
    }
}

bool validar_operacion(tipo_interfaz tipo_de_interfaz, t_identificador operacion){
    switch(tipo_de_interfaz) {
        case GENERICA:
            return operacion == IO_GEN_SLEEP;
        case STDIN:
            return operacion == IO_STDIN_READ;
        case STDOUT:
            return operacion == IO_STDOUT_WRITE;
        case DIALFS:
            return operacion == IO_FS_CREATE || operacion == IO_FS_DELETE || operacion == IO_FS_TRUNCATE || operacion == IO_FS_WRITE || operacion == IO_FS_READ;
        default:
            return false; // Si la interfaz no es válida, retornamos falso
    }
}

bool validar_existencia_nombre_interfaz(t_dictionary* diccionario, int operacion, char* nombre_interfaz, t_PCB* pcb){
    if(dictionary_has_key(interfaces, nombre_interfaz)){
        return true;
    }else{
        // ejemplo (IO_GEN_SLEEP)
        char* nombre_operacion = convertir_a_string(operacion);

        log_debug(logger, "PID: <%u> - PETICION_IO - operacion <%s> (Denegada porque no existe la interfaz (%s) en el Kernel)", pcb->pid, nombre_operacion, nombre_interfaz);
            
        char* motivo = obtener_motivo_salida(SALIDA_INVALID_INTERFACE, nombre_interfaz);
        mover_execute_a_exit(pcb, motivo); // aca dentro se actualiza el contexto de ejecucion
        free(motivo);

        return false;
    }      
}

bool validar_que_interfaz_admita_operacion(t_interfaz* interfaz, int operacion, char* nombre_interfaz, t_PCB* pcb){
    if(validar_operacion(interfaz->tipo, operacion)){
        return true;
    }else{
        // ejemplo (IO_GEN_SLEEP)
        char* nombre_operacion = convertir_a_string(operacion);

        log_debug(logger, "PID: <%u> - PETICION_IO - operacion <%s> (Denegada porque la interfaz (%s) no admite la operacion (%s))", pcb->pid, nombre_operacion, nombre_interfaz, nombre_operacion);

        char* operacion_string = convertir_a_string(operacion);    
        char* motivo = obtener_motivo_salida(SALIDA_INVALID_INTERFACE_OPERATION, operacion_string);
        mover_execute_a_exit(pcb, motivo);
        free(motivo);

        return false;
    }
}

void manejador_de_procesos_pendientes_io(void* arg){
    while(procesar_conexion_en_ejecucion){
        sem_wait(&sem_peticiones_io_por_procesar);
        sem_wait(&sem_interfaz_io_libre);

        t_interfaz* tmp = NULL;
        t_io_pendiente* pendientes_io = NULL;

        sem_wait(&mutex_diccionario_interfaces);
        t_list* lista_keys = dictionary_keys(interfaces); //

        for(int i = 0; i < dictionary_size(interfaces); i++){
            tmp = dictionary_get(interfaces, (char*)list_get(lista_keys, i));

            if(!tmp->ocupado && !queue_is_empty(tmp->cola)){
                
                tmp->ocupado = true;
                pendientes_io = (t_io_pendiente*) queue_pop(tmp->cola); //
                pendientes_io->ejecutando = true;
                mandar_a_procesar_io(tmp->socket, pendientes_io);

                sem_wait(&mutex_io_pendientes_ejecutando);
                    list_add(io_pendientes_ejecutando, (void*)pendientes_io);
                sem_post(&mutex_io_pendientes_ejecutando);
            }    
        }

        list_destroy(lista_keys);
        sem_post(&mutex_diccionario_interfaces);
    }
}

void mandar_a_procesar_io(int conexion_interfaz, t_io_pendiente* pendiente){
    switch(pendiente->operacion) {
        case IO_GEN_SLEEP:
            solicitar_IO_GEN_SLEEP(conexion_interfaz, pendiente);
            break;
        case IO_STDIN_READ:
            solicitar_IO_STDIN_READ(conexion_interfaz, pendiente);
            break;
        case IO_STDOUT_WRITE:
            solicitar_IO_STDOUT_WRITE(conexion_interfaz, pendiente);
            break;
        case IO_FS_CREATE:
            solicitar_IO_FS_CREATE(conexion_interfaz, pendiente);
            break;
        case IO_FS_DELETE:
            solicitar_IO_FS_DELETE(conexion_interfaz, pendiente);
            break;
        case IO_FS_TRUNCATE:
            solicitar_IO_FS_TRUNCATE(conexion_interfaz, pendiente);
            break;
        case IO_FS_WRITE:
            solicitar_IO_FS_WRITE(conexion_interfaz, pendiente);
            break;
        case IO_FS_READ:
            solicitar_IO_FS_READ(conexion_interfaz, pendiente);
            break;
    }
}

void solicitar_IO_GEN_SLEEP(int conexion_interfaz, t_io_pendiente* pendiente){
    // se enviar el pid del proceso | unidades de trabajo
    enviar_generico_doble_entero(conexion_interfaz, SOLICITUD_IO_GEN_SLEEP, pendiente->pid, pendiente->parametro_int_1);
}

void solicitar_IO_STDIN_READ(int conexion_interfaz, t_io_pendiente* pendiente){
    // se enviar el pid del proceso | tamanio en bytes 
    enviar_generico_doble_entero(conexion_interfaz, SOLICITUD_IO_STDIN_READ, pendiente->pid, pendiente->parametro_int_1);
    // se envia direcciones
    enviar_lista_peticiones_memoria(conexion_interfaz, IGNORAR_OP_CODE, pendiente->peticiones_memoria);
}

void solicitar_IO_STDOUT_WRITE(int conexion_interfaz, t_io_pendiente* pendiente){
    // se enviar el pid del proceso | tamanio en bytes 
    enviar_generico_doble_entero(conexion_interfaz, SOLICITUD_IO_STDOUT_WRITE, pendiente->pid, pendiente->parametro_int_1);
    // se envia direcciones
    //log_warning(logger, "Imprimimos peticiones para ver si un = basta");
    //printf("Imprimimos peticiones a Enviar para ver si un = basta \n");
    //imprimir_lista_peticion_memoria(pendiente->peticiones_memoria);
    enviar_lista_peticiones_memoria(conexion_interfaz, IGNORAR_OP_CODE, pendiente->peticiones_memoria);
}

void solicitar_IO_FS_CREATE(int conexion_interfaz, t_io_pendiente* pendiente){
    // se enviar el pid del proceso | nombre_archivo 
    envio_generico_entero_y_string(conexion_interfaz, SOLICITUD_IO_FS_CREATE, pendiente->pid, pendiente->parametro_string);
}

void solicitar_IO_FS_DELETE(int conexion_interfaz, t_io_pendiente* pendiente){
    // se enviar el pid del proceso | nombre_archivo 
    envio_generico_entero_y_string(conexion_interfaz, SOLICITUD_IO_FS_DELETE, pendiente->pid, pendiente->parametro_string);
}

void solicitar_IO_FS_TRUNCATE(int conexion_interfaz, t_io_pendiente* pendiente){
    // se enviar el pid del proceso | tamanio en bytes | nombre_archivo 
    enviar_generico_doble_entero_y_string(conexion_interfaz, SOLICITUD_IO_FS_TRUNCATE, pendiente->pid, pendiente->parametro_int_1, pendiente->parametro_string);
}

void solicitar_IO_FS_WRITE(int conexion_interfaz, t_io_pendiente* pendiente){
    // se enviar el pid del proceso | tamanio en bytes | puntero | nombre_archivo 
    enviar_generico_triple_entero_y_string(conexion_interfaz, SOLICITUD_IO_FS_WRITE, pendiente->pid, pendiente->parametro_int_1, pendiente->parametro_int_2, pendiente->parametro_string);
    // se envia direcciones
    enviar_lista_peticiones_memoria(conexion_interfaz, IGNORAR_OP_CODE, pendiente->peticiones_memoria);
}

void solicitar_IO_FS_READ(int conexion_interfaz, t_io_pendiente* pendiente){
    // se enviar el pid del proceso | tamanio en bytes | puntero | nombre_archivo 
    enviar_generico_triple_entero_y_string(conexion_interfaz, SOLICITUD_IO_FS_READ, pendiente->pid, pendiente->parametro_int_1, pendiente->parametro_int_2, pendiente->parametro_string);
    // se envia direcciones
    enviar_lista_peticiones_memoria(conexion_interfaz, IGNORAR_OP_CODE, pendiente->peticiones_memoria);
}


/////////////////////////////////////////////////////////////////////////////////////77
t_PCB* buscar_pcb_por_pid_y_remover(int pid, t_list* lista){
    int posicion = posicion_de_pcb_por_pid(pid, lista);
    if(posicion != -1){
       return (t_PCB*) list_remove(lista, posicion); 
    }else{
        return NULL;
    }
}

t_PCB* buscar_pcb_por_pid_y_obtener(int pid, t_list* lista){
    int posicion = posicion_de_pcb_por_pid(pid, lista);
    if(posicion != -1){
        return (t_PCB*) list_get(lista, posicion);
    }else{
        return NULL;
    }
}

int posicion_de_pcb_por_pid(int pid, t_list* lista){
    t_link_element **indirect = &lista->head;
    t_PCB* pcb = NULL;
    int contador = 0;

	while ((*indirect) != NULL) {
		pcb = (t_PCB*) (*indirect)->data;
        
        if( ((int)pcb->pid) == pid ){
            return contador;
        }
		indirect = &(*indirect)->next;
        contador++;
	}
    log_debug(logger, "funcion (posicion_de_pcb_por_pid) arrojo un -1 \n");
    contador = -1;

    return contador;
}

bool pid_pendiente_finalizacion(uint32_t pid, t_list* lista){
    sem_wait(&mutex_victimas_pendientes_io);

    // si se encuentra en la lista, es porque tiene pedido de captura
    // t_list* victimas_pendientes_io;
    int posicion = posicion_de_io_victima_por_pid(pid, lista);
    
    if(posicion != -1){
        sem_post(&mutex_victimas_pendientes_io);
        return true;
    }
    else{
        sem_post(&mutex_victimas_pendientes_io);
        return false;
    }
}

void remover_io_pendiente_en_ejecucion_protegida(uint32_t pid, t_list* lista){
    sem_wait(&mutex_io_pendientes_ejecutando);
        t_io_pendiente* pendiente = buscar_io_pendiente_por_pid_y_remover(pid, lista);
        pendiente->ejecutando = false;
        liberar_elemento_t_io_pendiente((void*)pendiente);
    sem_post(&mutex_io_pendientes_ejecutando);
}

///////////////////////////// Funciones para evitar repeticion de Logica en Conexiones IO //////////////////////////

void cambiar_estado_interfaz(t_dictionary* interfaces, char* nombre_interfaz){
    t_interfaz* interfaz_sleep = dictionary_get(interfaces, nombre_interfaz);
    interfaz_sleep->ocupado = false;
}

void gestor_procesos_io_pendientes_finalizacion(uint32_t pid, char* nombre_interfaz){
    sem_wait(&mutex_victimas_pendientes_io);
        t_io_victima* victima = buscar_io_victima_por_pid_y_remover(pid, victimas_pendientes_io);
    sem_post(&mutex_victimas_pendientes_io);
    
    //mandar de bloqueado a exit
    sem_wait(&mutex_cola_blocked);
        t_PCB* pcb_a_finalizar = buscar_pcb_por_pid_y_remover((int)pid, cola_blocked->elements);
    sem_post(&mutex_cola_blocked);

    char* motivo = obtener_motivo_salida(SALIDA_INTERRUPTED_BY_USER, NULL);
    mandar_a_exit(pcb_a_finalizar, motivo);
    free(motivo);
    sem_post(&sem_grado_multiprogramacion);

    // removemos y liberamos memoria de un (t_io_pendiente*)
    remover_io_pendiente_en_ejecucion_protegida(pid, io_pendientes_ejecutando);

    // cambiar el estado (sacando del diccionario) (tenemos el key nombre_interfaz)
    sem_wait(&mutex_diccionario_interfaces);
        cambiar_estado_interfaz(interfaces, nombre_interfaz);
    sem_post(&mutex_diccionario_interfaces);

    // post semaforo avisando que hay una interfaz de io disponible
    sem_post(&sem_interfaz_io_libre);

    liberar_elemento_t_io_victima((void*) victima); 

    if(victima->motivo_victima == INTERFAZ_DESCONECTADA){
        // nunca se dara si la interfaz fue desconectada porque no llegara nada
    }

    liberar_elemento_t_io_victima((void*) victima); 
}

void gestor_blocked_a_ready_segun_algoritmo(algoritmo algoritmo_elegido, t_PCB* pcb){
    // movemos el proceso a la lista de ready o ready aux (segun algoritmo)
    if((algoritmo_elegido == VRR) && (pcb->quantum != 0)){
        mover_blocked_a_ready_aux((int)pcb->pid);
    }
    else{
        mover_blocked_a_ready((int)pcb->pid);
    } 
}