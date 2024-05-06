#include "procesar_conexion.h"


void procesar_conexion_siendo_io_generica(void *args){
    char* nombre_modulo_server = "KERNEL";

    while (procesar_conexion_en_ejecucion) {
        int cod_op = recibir_operacion(conexion_kernel); // bloqueante
        log_info(logger, "Se recibió el cod operacion %d de el server %s", cod_op, nombre_modulo_server);
        switch (cod_op) {
            case SOLICITUD_IO_GEN_SLEEP:
                int pid, unidades_de_trabajo, unidades_totales;
                recibo_generico_doble_entero(conexion_kernel, &pid, &unidades_de_trabajo);
                log_info(logger, "PID: <%d> - Operacion: <IO_GEN_SLEEP> - Iniciada", pid);
                unidades_totales = unidades_de_trabajo * config->tiempo_unidad_trabajo;
                log_info(logger, "PID: <%d> - Operacion: <IO_GEN_SLEEP> - dormira (%d) milisegundos", pid, unidades_totales);
                hilo_dormir_milisegundos(unidades_totales);
                envio_generico_entero(conexion_kernel, SOLICITUD_IO_GEN_SLEEP_FINALIZADA, pid);
                log_info(logger, "PID: <%d> - Operacion : <IO_GEN_SLEEP> - Finalizada \n", pid);
                break;
            case -1:
                log_error(logger, "el server %s cerro la conexion", nombre_modulo_server);
                return; // finalizando hilo
            default:
                log_error(logger, "El codigo de operacion %d es incorrecto - %s", cod_op, nombre_modulo_server);
                return; // finalizando hilo
        }
    }

}