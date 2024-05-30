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
            {
                uint32_t tipo_interfaz;
                recibir_generico_entero_string(socket, &tipo_interfaz, &nombre_interfaz);
                log_info(logger_k, "Recibiendo nombre y tipo de Interfaz: (%s) \n", nombre_interfaz);
            
                sem_wait(&mutex_diccionario_interfaces);
                    agregar_interfaz(interfaces, nombre_interfaz, socket, (int)tipo_interfaz);
                sem_post(&mutex_diccionario_interfaces);
                
                envio_generico_op_code(socket, KERNEL_OK);
                
                sem_post(&sem_interfaz_io_libre);
                //free(nombre_interfaz);
                break;
            }
            case SOLICITUD_IO_GEN_SLEEP_FINALIZADA:
            {
                uint32_t pid = recibo_generico_entero(socket);
                
                log_info(logger, "PID: <%u> - Solicitud de IO_GEN_SLEEP Finalizada", pid);
                // movemos el proceso a la lista de ready                
                mover_blocked_a_ready((int)pid);

                // cambiar el estado (sacando del diccionario) (tenemos el key nombre_interfaz)
                sem_wait(&mutex_diccionario_interfaces);
                    t_interfaz* interfaz_sleep = dictionary_get(interfaces, nombre_interfaz);
                    interfaz_sleep->ocupado = false;
                    // interfaz_sleep->cola | el t_io_pendiente* asociado ya fue liberado y removido antes de realizar la llamada
                sem_post(&mutex_diccionario_interfaces);

                // post semaforo avisando que hay una interfaz de io disponible
                sem_post(&sem_interfaz_io_libre); // aviso que hay una instancia de io
                break;
            }
            case -1:
                log_error(logger_k, "Cliente (%s) desconectado de %s \n", nombre_interfaz, nombre_servidor);

                sem_wait(&mutex_diccionario_interfaces);
                    quitar_interfaz(interfaces, nombre_interfaz);
                sem_post(&mutex_diccionario_interfaces);
                
                free(nombre_interfaz);
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

    while (procesar_conexion_en_ejecucion) {
        int cod_op = recibir_operacion(conexion_cpu_dispatch); // bloqueante
        log_info(logger, "Se recibió el cod operacion %d de el server %s", cod_op, nombre_modulo_server);
        sem_wait(&mutex_conexion_cpu_dispatch);
        switch (cod_op) {
            case DESALOJO:
            {
                // TODO
                sem_wait(&mutex_proceso_en_ejecucion);
                    proceso_en_ejecucion = false;
                sem_post(&mutex_proceso_en_ejecucion);

                t_PCB* pcb = recibir_pcb(conexion_cpu_dispatch);
                sem_post(&mutex_conexion_cpu_dispatch);
                
                // IIIIII -> luego revisar si puede haber otros motivos de desalojo
                log_info(logger, "PID: <%u> - Desalojado por fin de Quantum", pcb->pid);

                mover_execute_a_ready(pcb);

                sem_post(&sem_cpu_disponible); 
                break;
            }
            case WAIT_KERNEL:
                // TODO
                sem_post(&mutex_conexion_cpu_dispatch);
                break;
            case SIGNAL_KERNEL:
                // TODO
                sem_post(&mutex_conexion_cpu_dispatch);
                break;
            case PETICION_IO: // mandamos a blocked, o sino a exit
            {
                // TODO
                sem_wait(&mutex_proceso_en_ejecucion);
                    proceso_en_ejecucion = false;
                sem_post(&mutex_proceso_en_ejecucion);

                t_PCB* pcb = recibir_pcb(conexion_cpu_dispatch);
                log_info(logger, "PID: <%u> - PETICION_IO", pcb->pid);

                verificar_tipo_interfaz(conexion_cpu_dispatch, pcb);
                
                //sem_post(&mutex_conexion_cpu_dispatch);
                //mover_execute_a_blocked(pcb); // aca dentro se actualiza el contexto de ejecucion
                //hilo_procesar_io_fake(3000);

                sem_post(&sem_cpu_disponible); 
                break;
            }
            case PROCESO_FINALIZADO:
            {
                sem_wait(&mutex_proceso_en_ejecucion);
                    proceso_en_ejecucion = false;
                sem_post(&mutex_proceso_en_ejecucion);
                
                t_PCB* pcb = recibir_pcb(conexion_cpu_dispatch);
                sem_post(&mutex_conexion_cpu_dispatch);
                
                mover_execute_a_exit(pcb, "SUCCESS");

                sem_post(&sem_cpu_disponible);

                sem_post(&sem_grado_multiprogramacion);
                break;
            }
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

    //mover_blocked_a_ready();

    free(milisegundos);
}
