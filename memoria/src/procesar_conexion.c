#include "procesar_conexion.h"

void procesar_conexion_general(void *args){
    t_procesar_conexion_args *casted_args = (t_procesar_conexion_args *)args;
    t_log* logger_m = casted_args->log; 
    int socket = casted_args->fd_cliente;
    char* nombre_servidor = casted_args->server_name;
    free(casted_args);

    while (procesar_conexion_en_ejecucion) {
        int cod_op = recibir_operacion(socket);
        log_info(logger_m, "Se recibió el cod operacion %d en %s", cod_op, nombre_servidor);
        switch (cod_op) {
            case HANDSHAKE:
                // Hacer algo si el código de operación es 1 (usar ENUM)
                char* modulo = recibir_handshake(socket);
                log_info(logger_m, "solicitud de Handshake del modulo: %s", modulo);
                log_info(logger_m, "respondiendo Handshake al modulo: %s \n", modulo);
                responder_handshake(socket);
                free(modulo);
                break;
            case SOLICITUD_TAMANIO_PAGINA: // KERNEL
                log_info(logger_m, "Solicitud de tamanio de pagina de CPU");

                log_info(logger_m, "Enviando a CPU tamanio de pagina (%d) \n", config->tam_pagina);
                enviar_generico_entero(socket, MEMORIA_OK, (uint32_t)config->tam_memoria);
                break;
            case NUEVO_PROCESO_MEMORIA: // KERNEL
                // se crea un proceso en memoria a partir de path (en memoria) enviado por el kernel
                log_info(logger_m, "solicitud de Nuevo Proceso del KERNEL \n");
                kernel_creacion_nuevo_proceso(socket);
                envio_generico_op_code(socket, MEMORIA_OK);
                break;
            case SOLICITAR_INTRUCCION_MEMORIA: // CPU
                // busca la intruccion pedida y la devuelve a la cpu
                log_info(logger_m, "solicitud de instruccion de CPU");
                uint32_t pid, program_counter;
                recibir_generico_doble_entero(socket,&pid, &program_counter);
                
                sem_wait(&mutex_lista_de_procesos);
                t_instruccion* intruccion = buscar_intruccion((int)pid, (int)program_counter);
                sem_post(&mutex_lista_de_procesos);
                
                log_info(logger_m, "PID: <%d> tiempo de retardo para envio de intruccion %d milisegundos", pid, config->retardo_respuesta);
                sleep_ms(config->retardo_respuesta);
                log_info(logger_m, "PID: <%d> enviando instruccion a CPU \n", pid);
                enviar_instruccion(socket, intruccion, IGNORAR_OP_CODE);
                break;
            case PROCESO_FINALIZADO_MEMORIA: // KERNEL
                // busca la intruccion pedida y la devuelve a la cpu
                log_info(logger_m, "solicitud de Finalizacion de proceso de KERNEL");
                uint32_t pid_a_finalizar = recibir_generico_entero(socket);

                log_info(logger_m, "liberando estructuras administrativas del proceso PID: <%d> \n", pid_a_finalizar);                
                sem_wait(&mutex_lista_de_procesos);
                t_proceso* proceso_a_eliminar = buscar_proceso_por_pid_y_remover((int)pid_a_finalizar, lista_de_procesos);
                sem_post(&mutex_lista_de_procesos);
                liberar_elemento_proceso((void*) proceso_a_eliminar);

                envio_generico_op_code(socket, MEMORIA_OK);
                break;
            case -1:
                log_error(logger_m, "Cliente desconectado de %s", nombre_servidor);
                //return EXIT_FAILURE -- si queremos terminar el server apenas alguien se desconecte
                //break;
                return; // terminamos la funcion si el cliente se desconecta
            default:
                log_error(logger_m, "El codigo de operacion %d es incorrecto - %s", cod_op, nombre_servidor);
                break;
        }
    }

    // libera la conexion si el cliente no la libero al terminar la funcion
    if(!procesar_conexion_en_ejecucion) liberar_conexion(socket);
}
