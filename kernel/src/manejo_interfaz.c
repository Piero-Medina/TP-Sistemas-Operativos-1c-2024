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
            break;
        case STDOUT:
            break;
        case DIALFS:
            break;
    }
}

void verificar_operacion_generica(int conexion, char* nombre_interfaz, t_PCB* pcb){
    int operacion = recibo_generico_op_code(conexion);

    t_interfaz* interfaz = NULL;

    if(operacion == IO_GEN_SLEEP){
        uint32_t unidades_genericas = recibo_generico_entero(conexion);
        sem_post(&mutex_conexion_cpu_dispatch); // terminamos de recibir todos los datos

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
           log_info(logger, "PID: <%u> - interceptado antes de bloquearse", pcb->pid);
           return; // salimos de la funcion
        }

        t_io_pendiente* pendiente_de_io = NULL;
        pendiente_de_io = inicializar_io_pendiente(pcb->pid, operacion, true, NULL, NULL, unidades_genericas, 0, 0, 0); //
            
        queue_push(interfaz->cola, (void*) pendiente_de_io);
        sem_post(&mutex_diccionario_interfaces);

        sem_post(&sem_peticiones_io_por_procesar);
    }
}

void verificar_operacion_stdin(int conexion, char* nombre_interfaz, int operacion){
    if(operacion == IO_STDIN_READ){
        // validar que exista el nombre de interfaz
        // validar que admite la operacion

        // recien validamos aca para no cortar el envio de la cpu
        // recibimos todo y dejamos al socket libre para que trabaje
    }
}

void verificar_operacion_stdout(int conexion, char* nombre_interfaz, int operacion){
    if(operacion == IO_STDIN_READ){
        // validar que exista el nombre de interfaz
        // validar que admite la operacion

        // recien validamos aca para no cortar el envio de la cpu
        // recibimos todo y dejamos al socket libre para que trabaje
    }
}

void verificar_operacion_dialFs(int conexion, char* nombre_interfaz, int operacion){
    switch (operacion){
        case IO_FS_CREATE:
            // validar que exista el nombre de interfaz
            // validar que admite la operacion

            // recien validamos aca para no cortar el envio de la cpu
             // recibimos todo y dejamos al socket libre para que trabaje
            break;
        case IO_FS_DELETE:
            //  
            break;
        case IO_FS_TRUNCATE:
            // 
            break;
        case IO_FS_WRITE:
            // 
            break;
        case IO_FS_READ:
            // 
            break;
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

        log_info(logger, "PID: <%u> - PETICION_IO - operacion <%s> (Denegada porque no existe la interfaz (%s) en el Kernel)", pcb->pid, nombre_operacion, nombre_interfaz);
            
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

        log_info(logger, "PID: <%u> - PETICION_IO - operacion <%s> (Denegada porque la interfaz (%s) no admite la operacion (%s))", pcb->pid, nombre_operacion, nombre_interfaz, nombre_operacion);
            
        char* motivo = obtener_motivo_salida(SALIDA_INVALID_INTERFACE_OPERATION, "IO_GEN_SLEEP");
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
                mandar_a_procesar_io(tmp->socket, pendientes_io);
                liberar_elemento_t_io_pendiente((void*) pendientes_io);
            }    
        }
        list_destroy(lista_keys);
        sem_post(&mutex_diccionario_interfaces);


        // si no la encontramos capas la interfaz fue desconectada
        // esto lo delegamos a la funcion que procesa conexion, para que cuando
        // una interfaz se desconecte busque a todos los procesos en la cola de bloqueados que tenian
        // pendiente trabajar con esa interfaz y los mandamos a exit.

    }
}

void mandar_a_procesar_io(int conexion_interfaz, t_io_pendiente* pendiente){
    switch(pendiente->operacion) {
        case IO_GEN_SLEEP:
            solicitar_IO_GEN_SLEEP(conexion_interfaz, pendiente);
            break;
        case IO_STDIN_READ:
            //TODO
            break;
        case IO_STDOUT_WRITE:
            //TODO
            break;
        case IO_FS_CREATE:
            //TODO
            break;
        case IO_FS_DELETE:
            //TODO
            break;
        case IO_FS_TRUNCATE:
            //TODO
            break;
        case IO_FS_WRITE:
            //TODO
            break;
        case IO_FS_READ:
            //TODO
            break;
    }
}

void solicitar_IO_GEN_SLEEP(int conexion_interfaz, t_io_pendiente* pendiente){
    // se enviar el pid del proceso y las unidades de trabajo
    envio_generico_doble_entero(conexion_interfaz, SOLICITUD_IO_GEN_SLEEP, pendiente->pid, pendiente->parametro_int_1);
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
    log_error(logger, "funcion (posicion_de_pcb_por_pid) arrojo un -1 \n");
    contador = -1;

    return contador;
}

bool pid_pendiente_finalizacion(uint32_t pid, t_list* lista){
    sem_wait(&mutex_victimas_pendientes_io);
    
    int* tmp = NULL;
    int pid_cast = (int)pid;
    int tamanio = list_size(lista);

    for (int i = 0; i < tamanio; i++){
        tmp = (int*) list_get(lista, i);
        if(*tmp == pid_cast){
            list_remove_and_destroy_element(lista, i, free);
            sem_post(&mutex_victimas_pendientes_io);
            return true;
        }
    }
    sem_post(&mutex_victimas_pendientes_io);
    return false;
}