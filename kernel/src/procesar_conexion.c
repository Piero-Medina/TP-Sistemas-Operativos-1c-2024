#include "procesar_conexion.h"

void procesar_conexion_io(void *args){
    t_procesar_conexion_args *casted_args = (t_procesar_conexion_args *)args;
    t_log* logger_k = casted_args->log; 
    int socket = casted_args->fd_cliente;
    char* nombre_servidor = casted_args->server_name;
    free(casted_args);

    while (procesar_conexion_en_ejecucion) {
        int cod_op = recibir_operacion(socket);
        log_info(logger_k, "Se recibió el cod operacion %d en %s", cod_op, nombre_servidor);
        switch (cod_op) {
            case HANDSHAKE:
                // Hacer algo si el código de operación es 1 (usar ENUM)
                char* modulo = recibir_handshake(socket);
                log_info(logger_k, "solicitud de Handshake del modulo: %s", modulo);
                log_info(logger_k, "respondiendo Handshake al modulo: %s \n", modulo);
                responder_handshake(socket);
                free(modulo);
                break;
            case -1:
                log_error(logger_k, "Cliente desconectado de %s", nombre_servidor);
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

    while (procesar_conexion_en_ejecucion) {
        int cod_op = recibir_operacion(conexion_cpu_dispatch); // bloqueante
        log_info(logger, "Se recibió el cod operacion %d de el server %s", cod_op, nombre_modulo_server);
        sem_wait(&mutex_conexion_cpu_dispatch);
        switch (cod_op) {
            case WAIT:
                // TODO
                sem_post(&mutex_conexion_cpu_interrupt);
                break;
            case SIGNAL:
                // TODO
                sem_post(&mutex_conexion_cpu_interrupt);
                break;
            case PETICION_IO:
                // TODO
                t_PCB* pcb = recibir_pcb(conexion_cpu_dispatch);
                sem_post(&mutex_conexion_cpu_interrupt);
                // aca dentro se actualiza el contexto de ejecucion
                mover_execute_a_blocked(pcb);
                // avisamos que la cpu ya esta disponible para ejecutar otro proceso
                sem_post(&sem_cpu_disponible); 
                // dormimos 3 segundos simulando una operacion de IO
                hilo_dormir_milisegundos(3000);
                
                mover_blocked_a_ready();
                break;
            case PROCESO_FINALIZADO:
                // TODO
                // sem_post(grado de multiprogramacion)
                sem_post(&mutex_conexion_cpu_interrupt);
                break;
            case -1:
                log_error(logger, "el server %s cerro la conexion", nombre_modulo_server);
                sem_post(&mutex_conexion_cpu_interrupt);
                return; // finalizando hilo
            default:
                log_error(logger, "El codigo de operacion %d es incorrecto - %s", cod_op, nombre_modulo_server);
                sem_post(&mutex_conexion_cpu_interrupt);
                return; // finalizando hilo
        }
    }

}
