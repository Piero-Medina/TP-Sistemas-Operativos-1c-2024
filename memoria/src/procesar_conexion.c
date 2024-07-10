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
                char* modulo = recibir_handshake(socket);
                log_info(logger_m, "solicitud de Handshake del modulo: %s", modulo);
                log_info(logger_m, "respondiendo Handshake al modulo: %s \n", modulo);
                responder_handshake(socket);
                free(modulo);
                break;
            case SOLICITUD_TAMANIO_PAGINA: // CPU
                log_info(logger_m, "Solicitud de tamanio de pagina de CPU");

                log_info(logger_m, "Enviando a CPU tamanio de pagina (%d) \n", config->tam_pagina);
                enviar_generico_entero(socket, MEMORIA_OK, (uint32_t)config->tam_pagina);
                break;
            case NUEVO_PROCESO_MEMORIA: // KERNEL
            {
                // se crea un proceso en memoria a partir de path (en memoria) enviado por el kernel
                log_info(logger_m, "solicitud de Nuevo Proceso del KERNEL");
                kernel_creacion_nuevo_proceso(socket);
                envio_generico_op_code(socket, MEMORIA_OK);
                break;
            }
            case SOLICITAR_INTRUCCION_MEMORIA: // CPU
            {
                // busca la intruccion pedida y la devuelve a la cpu
                log_info(logger_m, "solicitud de instruccion de CPU");
                uint32_t pid, program_counter;
                recibir_generico_doble_entero(socket,&pid, &program_counter);
                
                sem_wait(&mutex_lista_de_procesos);
                t_instruccion* intruccion = buscar_intruccion((int)pid, (int)program_counter);
                sem_post(&mutex_lista_de_procesos);
                
                log_info(logger_m, "PID: <%d> tiempo de retardo para envio de intruccion (%d) milisegundos", pid, config->retardo_respuesta);
                sleep_ms(config->retardo_respuesta);
                log_info(logger_m, "PID: <%d> enviando instruccion a CPU \n", pid);

                enviar_instruccion(socket, intruccion, IGNORAR_OP_CODE);
                break;
            }
            case SOLICITUD_MARCO_MEMORIA: // CPU
            {
                log_info(logger_m, "solicitud de Marco");
                uint32_t pid, numero_pagina;
                recibir_generico_doble_entero(socket, &pid, &numero_pagina);

                sem_wait(&mutex_lista_de_procesos);
                    int32_t marco = buscar_marco((int) pid, (int) numero_pagina);
                sem_post(&mutex_lista_de_procesos);
                
                log_info(logger_m, "PID: <%d> - Pagina: <%d> - Marco: <%"PRId32">", pid, numero_pagina, marco);

                log_info(logger_m, "PID: <%d> tiempo de retardo para envio de peticion (%d) milisegundos", pid, config->retardo_respuesta);
                sleep_ms(config->retardo_respuesta);

                if(marco == -1){
                    log_info(logger_m, "PID: <%d> Marco no existente \n", pid);
                    enviar_generico_int32(socket, SEGMENTATION_FAULT, marco);
                    break;
                }

                log_info(logger_m, "PID: <%d> enviando peticion a CPU \n", pid);
                enviar_generico_int32(socket, MEMORIA_OK, marco);
                break;
            }
            case SOLICITUD_RESIZE_MEMORIA: // CPU
            {   
                log_info(logger_m, "solicitud Resize");
                uint32_t pid, tamanio;
                recibir_generico_doble_entero(socket, &pid, &tamanio);

                sem_wait(&mutex_lista_de_procesos);
                    t_proceso* proceso = buscar_proceso((int) pid);
                sem_post(&mutex_lista_de_procesos);
                
                log_info(logger_m, "PID: <%u> - Tamaño Actual: <%d> - Tamanio Resize: <%u>", pid, proceso->tamanio, tamanio);

                int estado = resize_proceso(pid, tamanio);

                log_info(logger_m, "PID: <%d> tiempo de retardo para envio de peticion (%d) milisegundos", pid, config->retardo_respuesta);
                sleep_ms(config->retardo_respuesta);

                if(estado == OUT_OF_MEMORY){
                    log_info(logger_m, "PID: <%u> - ERROR (OUT_OF_MEMORY) al intentar ampliar proceso a (%u) \n", pid, tamanio);
                    envio_generico_op_code(socket, OUT_OF_MEMORY);
                    break;
                }

                log_info(logger_m, "PID: <%u> - Nuevo tamanio <%u> \n", pid, tamanio);
                envio_generico_op_code(socket, MEMORIA_OK); 
                break;
            }
            case SOLICITUD_LECTURA_MEMORIA: // GENERAL
            {
                log_info(logger_m, "solicitud Lectura Memoria");

                uint32_t direccion_fisica, bytes, pid;
                recibir_generico_triple_entero(socket, &direccion_fisica, &bytes, &pid);

                log_info(logger_m, "PID: <%u> - Accion: <LEER> - Direccion fisica: <%u> - Tamaño <%u>", pid, direccion_fisica, bytes);

                log_info(logger_m, "PID: <%d> tiempo de retardo para envio de peticion (%d) milisegundos", pid, config->retardo_respuesta);
                sleep_ms(config->retardo_respuesta);

                sem_wait(&mutex_memoria_real);
                    void* data = get_valor_memoria(direccion_fisica, bytes);
                sem_post(&mutex_memoria_real);

                char* string_leido = convertir_a_cadena_nueva(data, bytes);
                log_info(logger, "PID: <%u> - Acción: <LEER> - Valor: <%s> \n", pid, string_leido);
                free(string_leido);

                enviar_data(socket, IGNORAR_OP_CODE, data, bytes);

                free(data);
                break;
            }
            case SOLICITUD_ESCRITURA_MEMORIA: // GENERAL
            {
                log_info(logger_m, "solicitud Escritura Memoria");

                uint32_t direccion_fisica, pid, bytes;
                recibir_generico_doble_entero(socket, &direccion_fisica, &pid);
                ignorar_op_code(socket);
                void* data = recibir_data(socket, &bytes);

                log_info(logger_m, "PID: <%u> - Accion: <ESCRIBIR> - Direccion fisica: <%u> - Tamaño <%u>", pid, direccion_fisica, bytes);

                log_info(logger_m, "PID: <%d> tiempo de retardo para envio de peticion (%d) milisegundos", pid, config->retardo_respuesta);
                sleep_ms(config->retardo_respuesta);

                sem_wait(&mutex_memoria_real);
                    set_valor_en_memoria(direccion_fisica, data, bytes);
                sem_post(&mutex_memoria_real);

                char* string_escrito = convertir_a_cadena_nueva(data, bytes);
                log_info(logger, "PID: <%u> - Acción: <ESCRIBIR> - Valor: <%s>", pid, string_escrito);
                free(string_escrito);

                envio_generico_op_code(socket, MEMORIA_OK);

                free(data);
                break;
            }
            case PROCESO_FINALIZADO_MEMORIA: // KERNEL
            {
                // busca la intruccion pedida y la devuelve a la cpu
                log_info(logger_m, "solicitud de Finalizacion de proceso de KERNEL");
                uint32_t pid = recibir_generico_entero(socket);

                log_info(logger_m, "PID: <%d> tiempo de retardo para envio de peticion (%d) milisegundos", pid, config->retardo_respuesta);
                sleep_ms(config->retardo_respuesta);

                log_info(logger_m, "liberando estructuras administrativas del proceso PID: <%d>", pid);                
                liberar_proceso(pid);

                envio_generico_op_code(socket, MEMORIA_OK);
                break;
            }
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
