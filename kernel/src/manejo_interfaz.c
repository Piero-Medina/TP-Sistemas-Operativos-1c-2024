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
        int unidades_genericas = recibo_generico_entero(conexion);
        sem_post(&mutex_conexion_cpu_dispatch); // terminamos de recibir todos los datos

        // validar que exista el nombre de interfaz
        sem_wait(&mutex_diccionario_interfaces);
        if(!dictionary_has_key(interfaces, nombre_interfaz)){
            sem_post(&mutex_diccionario_interfaces);
            log_info(logger, "PID: <%d> - PETICION_IO - operacion <IO_GEN_SLEEP> (Denegada porque no existe la interfaz (%s) en el Kernel)", pcb->pid, nombre_interfaz);
            mover_execute_a_exit(pcb, "INTERFAZ SOLICITADA NO EXISTE"); // aca dentro se actualiza el contexto de ejecucion
            return;
        }
        interfaz = (t_interfaz*) dictionary_get(interfaces, nombre_interfaz);
        
        // validar que admite la operacion
        if(!validar_operacion(interfaz->tipo, operacion)){
            sem_post(&mutex_diccionario_interfaces);
            log_info(logger, "PID: <%d> - PETICION_IO - operacion <IO_GEN_SLEEP> (Denegada porque la interfaz (%s) no admite la operacion)", pcb->pid, nombre_interfaz);
            mover_execute_a_exit(pcb, "INTERFAZ SOLCITADA NO ADMITE OPERACION PEDIDA");
            return;
        }
    
        mover_execute_a_blocked(pcb); // aca dentro se actualiza el contexto de ejecucion

        t_io_pendiente* pendiente_de_io = NULL;
        pendiente_de_io = inicializar_io_pendiente(pcb->pid, operacion, true, NULL, unidades_genericas, 0, 0, 0); //
            
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


t_io_pendiente* inicializar_io_pendiente(int pid, int operacion, bool interfaz_ocupada, char* param_string, int param_int_1, int param_int_2, int param_int_3, int param_int_4){
    t_io_pendiente *nueva_io_pendiente = malloc(sizeof(t_io_pendiente));
    
    nueva_io_pendiente->pid = pid;
    nueva_io_pendiente->operacion = operacion;
    nueva_io_pendiente->interfaz_ocupada = interfaz_ocupada;
    if (param_string == NULL) {
        nueva_io_pendiente->parametro_string = NULL;
    } else {
        nueva_io_pendiente->parametro_string = strdup(param_string); // copia dinamica que luego se tiene que liberar
    }
    nueva_io_pendiente->parametro_int_1 = param_int_1;
    nueva_io_pendiente->parametro_int_2 = param_int_2;
    nueva_io_pendiente->parametro_int_3 = param_int_3;
    nueva_io_pendiente->parametro_int_4 = param_int_4;
    
    return nueva_io_pendiente;
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
                liberar_elemento_io_pendiente((void*) pendientes_io);
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


t_PCB* buscar_pcb_por_pid_y_remover(int pid, t_list* lista){
    int posicion = posicion_de_pcb_por_pid(pid, lista);
    return (t_PCB*) list_remove(lista, posicion);
}

int posicion_de_pcb_por_pid(int pid, t_list* lista){
    t_link_element **indirect = &lista->head;
    t_PCB* pcb = NULL;
    int contador = 0;

	while ((*indirect) != NULL) {
		pcb = (t_PCB*) (*indirect)->data;
        
        if(pcb->pid == pid){
            return contador;
        }
		indirect = &(*indirect)->next;
        contador++;
	}
    log_error(logger, "funcion (posicion_de_pcb_por_pid) arrojo un -1 \n");
    contador = -1;

    return contador;
}