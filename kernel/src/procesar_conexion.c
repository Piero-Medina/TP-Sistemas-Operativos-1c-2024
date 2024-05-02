#include "procesar_conexion.h"

void procesar_conexion_io(void *args){
    t_procesar_conexion_args *casted_args = (t_procesar_conexion_args *)args;
    t_log* logger_k = casted_args->log; 
    int socket = casted_args->fd_cliente;
    char* nombre_servidor = casted_args->server_name;
    free(casted_args);

    char* nombre_interfaz = NULL;

    while (procesar_conexion_en_ejecucion) {
        int cod_op = recibir_operacion(socket);
        log_info(logger_k, "Se recibió el cod operacion %d en %s", cod_op, nombre_servidor);
        switch (cod_op) {
            case HANDSHAKE:
                char* modulo = recibir_handshake(socket);
                log_info(logger_k, "solicitud de Handshake del modulo: %s", modulo);
                log_info(logger_k, "respondiendo Handshake al modulo: %s \n", modulo);
                responder_handshake(socket); 
                free(modulo);
                break;
            case REGISTRO_INTERFAZ:
                int tipo_interfaz = 0;
                recibir_generico_entero_string(socket, &tipo_interfaz, &nombre_interfaz);
                log_info(logger_k, "recibiendo nombre y tipo de Interfaz: %s \n", nombre_interfaz);
            
                sem_wait(&mutex_diccionario_interfaces);
                    agregar_interfaz(interfaces, nombre_interfaz, socket, tipo_interfaz);
                sem_post(&mutex_diccionario_interfaces);
                
                envio_generico_op_code(socket, KERNEL_OK);
                //free(nombre_interfaz);
                break;
            case -1:
                log_error(logger_k, "Cliente desconectado de %s \n", nombre_servidor);

                sem_wait(&mutex_diccionario_interfaces);
                    quitar_interfaz(interfaces, nombre_interfaz);
                sem_post(&mutex_diccionario_interfaces);
                
                free(nombre_interfaz);
                //return EXIT_FAILURE -- si queremos terminar el server apenas alguien se desconecte
                //break;
                return; // terminamos la funcion si el cliente se desconecta
            default:
                log_error(logger_k, "El codigo de operacion %d es incorrecto - %s", cod_op, nombre_servidor);
                break;
        }
    }

    // libera la conexion si el cliente no la libero al terminar la funcion
    if(!procesar_conexion_en_ejecucion) liberar_conexion(socket);
}


void procesar_conexion_cpu_dispatch(void *args){
    char* nombre_modulo_server = "CPU_DISPATCH";
    //t_PCB* pcb = NULL;

    while (procesar_conexion_en_ejecucion) {
        int cod_op = recibir_operacion(conexion_cpu_dispatch); // bloqueante
        log_info(logger, "Se recibió el cod operacion %d de el server %s", cod_op, nombre_modulo_server);
        sem_wait(&mutex_conexion_cpu_dispatch);
        switch (cod_op) {
            case DESALOJO:
                // TODO
                sem_wait(&mutex_proceso_en_ejecucion);
                    proceso_en_ejecucion = false;
                sem_post(&mutex_proceso_en_ejecucion);

                t_PCB* pcb_desalojada = recibir_pcb(conexion_cpu_dispatch);
                sem_post(&mutex_conexion_cpu_dispatch);
                
                // IIIIII -> luego revisar si puede haber otros motivos de desalojo
                log_info(logger, "PID: <%d> - Desalojado por fin de Quantum", pcb_desalojada->pid);

                mover_execute_a_ready(pcb_desalojada);

                sem_post(&sem_cpu_disponible); 
                break;
            case WAIT_KERNEL:
                // TODO
                sem_post(&mutex_conexion_cpu_dispatch);
                break;
            case SIGNAL_KERNEL:
                // TODO
                sem_post(&mutex_conexion_cpu_dispatch);
                break;
            case PETICION_IO:
                // TODO
                sem_wait(&mutex_proceso_en_ejecucion);
                    proceso_en_ejecucion = false;
                sem_post(&mutex_proceso_en_ejecucion);

                t_PCB* pcb_io = recibir_pcb(conexion_cpu_dispatch);

                /*
                    cuando este establecida la interfaz liberamos esta parte
                //int tipo_de_interfaz = recibo_generico_op_code(conexion_cpu_dispatch);
                //char* nombre_interfaz = recibir_generico_string(conexion_cpu_dispatch, &nombre_interfaz);
                //verificar_tipo_interfaz(conexion_cpu_dispatch, tipo_de_interfaz, nombre_interfaz);
                */
                sem_post(&mutex_conexion_cpu_dispatch);

                mover_execute_a_blocked(pcb_io); // aca dentro se actualiza el contexto de ejecucion
                
                hilo_procesar_io_fake(3000);

                sem_post(&sem_cpu_disponible); 
                break;
            case PROCESO_FINALIZADO:
                sem_wait(&mutex_proceso_en_ejecucion);
                    proceso_en_ejecucion = false;
                sem_post(&mutex_proceso_en_ejecucion);
                
                t_PCB* pcb_fin = recibir_pcb(conexion_cpu_dispatch);
                sem_post(&mutex_conexion_cpu_dispatch);
                
                mover_execute_a_exit(pcb_fin);

                sem_post(&sem_cpu_disponible);

                sem_post(&sem_grado_multiprogramacion);
                break;
            case -1:
                log_error(logger, "el server %s cerro la conexion", nombre_modulo_server);
                sem_post(&mutex_conexion_cpu_dispatch);
                return; // finalizando hilo
            default:
                log_error(logger, "El codigo de operacion %d es incorrecto - %s", cod_op, nombre_modulo_server);
                sem_post(&mutex_conexion_cpu_dispatch);
                return; // finalizando hilo
        }
    }

}


void hilo_procesar_io_fake(int milisegundos){
    pthread_t hilo_durmicion;
    int* milisegundos_ptr = malloc(sizeof(int));
    *milisegundos_ptr = milisegundos;
    pthread_create(&hilo_durmicion, NULL, (void*)procesar_io_fake, (void*) milisegundos_ptr);
    pthread_detach(hilo_durmicion);
}

void procesar_io_fake(void* args){
    int* milisegundos = (int*) args;
    sleep_ms(*milisegundos);

    mover_blocked_a_ready();

    free(milisegundos);
}