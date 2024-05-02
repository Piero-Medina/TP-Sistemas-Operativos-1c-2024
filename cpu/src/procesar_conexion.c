#include "procesar_conexion.h"

void procesar_conexion_kernel(void *args){
    t_procesar_conexion_args *casted_args = (t_procesar_conexion_args *)args;
    t_log* logger_c = casted_args->log; 
    int socket = casted_args->fd_cliente;
    char* nombre_servidor = casted_args->server_name;
    free(casted_args);

    while (procesar_conexion_en_ejecucion) {
        int cod_op = recibir_operacion(socket);
        log_info(logger_c, "Se recibió el cod operacion %d en %s", cod_op, nombre_servidor);
        switch (cod_op) {
            case HANDSHAKE:
                char* modulo = recibir_handshake(socket);
                log_info(logger_c, "solicitud de Handshake del modulo: %s", modulo);
                log_info(logger_c, "respondiendo Handshake al modulo: %s \n", modulo);
                responder_handshake(socket);
                free(modulo);
                break;
            case EJECUTAR_PROCESO:
                log_info(logger_c, "Solicitud de EJECUTAR_PROCESO");
                t_PCB* pcb_a_ejecutar = recibir_pcb(socket);
                log_info(logger_c, "Iniciando ciclo de instruccion para PID <%d>", pcb_a_ejecutar->pid);
                ejecutar_ciclo_de_instruccion(socket, pcb_a_ejecutar);
                log_info(logger_c, "finalizando ciclo de instruccion para PID <%d>", pcb_a_ejecutar->pid);
                liberar_PCB(pcb_a_ejecutar);
                break;
            case DESALOJO:
                log_info(logger_c, "Solicitud de DESALOJO");
                sem_wait(&mutex_desalojo);
                    desalojo = true;
                sem_post(&mutex_desalojo);
                break;
            case -1:
                log_error(logger_c, "Cliente desconectado de %s", nombre_servidor);
                //return EXIT_FAILURE -- si queremos terminar el server apenas alguien se desconecte
                //break;
                return; // terminamos la funcion si el cliente se desconecta
            default:
                log_error(logger_c, "El codigo de operacion %d es incorrecto - %s", cod_op, nombre_servidor);
                break;
        }
    }
    
    // libera la conexion si el cliente no la libero al terminar la funcion
    if(!procesar_conexion_en_ejecucion) liberar_conexion(socket);
}
