#include "procesar_conexion.h"


void procesar_conexion_siendo_io_generica(void *args){
    log_info(logger, "interfaz en GENERICA \n");
    char* nombre_modulo_server = "KERNEL";

    while (procesar_conexion_en_ejecucion) {
        int cod_op = recibir_operacion(conexion_kernel); // bloqueante
        log_info(logger, "Se recibió el cod operacion %d de el server %s", cod_op, nombre_modulo_server);
        switch (cod_op) {
            case SOLICITUD_IO_GEN_SLEEP:
                uint32_t pid, unidades_de_trabajo, unidades_totales;
                recibir_generico_doble_entero(conexion_kernel, &pid, &unidades_de_trabajo);

                log_info(logger, "PID: <%u> - Operacion: <IO_GEN_SLEEP> - Iniciada", pid);
                unidades_totales = unidades_de_trabajo * config->tiempo_unidad_trabajo;
                
                log_info(logger, "PID: <%u> - Operacion: <IO_GEN_SLEEP> - dormira (%u) milisegundos", pid, unidades_totales);
                hilo_dormir_milisegundos((int) unidades_totales);
                
                enviar_generico_entero(conexion_kernel, SOLICITUD_IO_GEN_SLEEP_FINALIZADA, pid);
                log_info(logger, "PID: <%u> - Operacion: <IO_GEN_SLEEP> - Finalizada \n", pid);
                break;
            case -1:
                log_error(logger, "el server %s cerro la conexion", nombre_modulo_server);
                return; // finalizando hilo
            default:
                log_error(logger, "El codigo de operacion %u es incorrecto - %s", cod_op, nombre_modulo_server);
                return; // finalizando hilo
        }
    }

}


void procesar_conexion_siendo_io_dialFs(void *args){
    log_info(logger, "interfaz en modo DIALFS \n");
    char* nombre_modulo_server = "KERNEL";

    while (procesar_conexion_en_ejecucion) {
        int cod_op = recibir_operacion(conexion_kernel); // bloqueante
        log_info(logger, "Se recibió el cod operacion %d de el server %s", cod_op, nombre_modulo_server);
        switch (cod_op) {
            case SOLICITUD_IO_FS_CREATE:
            {
                uint32_t pid;
                char* nombre_archivo;
                recibir_generico_entero_string(conexion_kernel, &pid, &nombre_archivo);
                log_info(logger, "PID: <%u> - Operacion: <IO_FS_CREATE> - Iniciada", pid);

                log_info(logger, "PID: <%u> - Crear Archivo: <%s>", pid, nombre_archivo);
                estado_interfaz estado_fs = crear_archivo(nombre_archivo);
                if(estado_fs == TODO_OK) log_info(logger, "PID: <%u> - Crear Archivo: <%s> TODO_OK", pid, nombre_archivo);
                if(estado_fs == SIN_ESPACIO)log_info(logger, "PID: <%u> - Crear Archivo: <%s> SIN_ESPACIO", pid, nombre_archivo);
                   
                enviar_generico_doble_entero(conexion_kernel, SOLICITUD_IO_FS_CREATE_FINALIZADA, pid, (uint32_t)estado_fs);
                log_info(logger, "PID: <%u> - Operacion: <IO_FS_CREATE> - Finalizada \n", pid);
                free(nombre_archivo);
                break;
            }
            case SOLICITUD_IO_FS_DELETE:
            {
                uint32_t pid;
                char* nombre_archivo;
                recibir_generico_entero_string(conexion_kernel, &pid, &nombre_archivo);
                log_info(logger, "PID: <%u> - Operacion: <IO_FS_DELETE> - Iniciada", pid);

                log_info(logger, "PID: <%u> - Eliminar Archivo: <%s>", pid, nombre_archivo);
                estado_interfaz estado_fs = eliminar_archivo(nombre_archivo);
                if(estado_fs == TODO_OK) log_info(logger, "PID: <%u> - Eliminar Archivo: <%s> TODO_OK", pid, nombre_archivo);
                if(estado_fs == ARCHIVO_NO_EXISTE)log_info(logger, "PID: <%u> - Eliminar Archivo: <%s> ARCHIVO_NO_EXISTE", pid, nombre_archivo);
                   
                enviar_generico_doble_entero(conexion_kernel, SOLICITUD_IO_FS_DELETE_FINALIZADA, pid, estado_fs);
                log_info(logger, "PID: <%u> - Operacion: <IO_FS_DELETE> - Finalizada \n", pid);
                free(nombre_archivo);
                break;
            }
            case SOLICITUD_IO_FS_TRUNCATE:
            {
                uint32_t pid, tamanio;
                char* nombre_archivo;
                recibir_generico_doble_entero_y_string(conexion_kernel, &pid, &tamanio, &nombre_archivo);
                log_info(logger, "PID: <%u> - Operacion: <IO_FS_TRUNCATE> - Iniciada", pid);

                log_info(logger, "PID: <%u> - Truncar Archivo: <%s> - Tamanio: <%u>", pid, nombre_archivo, tamanio);
                estado_interfaz estado_fs = truncar_archivo(nombre_archivo, (int) tamanio, (int) pid);
                if(estado_fs == TODO_OK)log_info(logger, "PID: <%u> - Truncar Archivo: <%s> TODO_OK", pid, nombre_archivo);
                if(estado_fs == SIN_ESPACIO)log_info(logger, "PID: <%u> - Truncar Archivo: <%s> SIN_ESPACIO", pid, nombre_archivo);
                if(estado_fs == NO_PROCESAR)log_info(logger, "PID: <%u> - Truncar Archivo: <%s> NO_PROCESAR", pid, nombre_archivo);

                enviar_generico_doble_entero(conexion_kernel, SOLICITUD_IO_FS_TRUNCATE_FINALIZADA, pid, estado_fs);
                log_info(logger, "PID: <%d> - Operacion: <IO_FS_TRUNCATE> - Finalizada \n", pid);
                free(nombre_archivo);
                break;
            }
            case SOLICITUD_IO_FS_WRITE:
            {
                uint32_t pid, bytes, puntero;
                char* nombre_archivo;
                recibir_generico_triple_entero_y_string(conexion_kernel, &pid, &bytes, &puntero, &nombre_archivo);
                
                ignorar_op_code(conexion_kernel);
                t_list* direcciones = recibir_lista_peticiones_memoria(conexion_kernel);

                log_info(logger, "PID: <%u> - Operacion: <IO_FS_WRITE> - Iniciada", pid);

                log_info(logger, "PID: <%u> - Escribir Archivo: <%s> - Tamanio a Escribir: <%u> - Puntero Archivo: <%u>", pid, nombre_archivo, bytes, puntero);
                estado_interfaz estado_fs = escribir_archivo(nombre_archivo, bytes, puntero, direcciones, pid);
                if(estado_fs == TODO_OK)log_info(logger, "PID: <%u> - Escribir Archivo: <%s> TODO_OK", pid, nombre_archivo);
                if(estado_fs == PUNTERO_INVALIDO)log_info(logger, "PID: <%u> - Escribir Archivo: <%s> PUNTERO_INVALIDO", pid, nombre_archivo);
                if(estado_fs == ARCHIVO_NO_EXISTE)log_info(logger, "PID: <%u> - Escribir Archivo: <%s> ARCHIVO_NO_EXISTE", pid, nombre_archivo);
                if(estado_fs == ESCRITURA_INVALIDA)log_info(logger, "PID: <%u> - Escribir Archivo: <%s> ESCRITURA_INVALIDA", pid, nombre_archivo);

                enviar_generico_doble_entero(conexion_kernel, SOLICITUD_IO_FS_WRITE_FINALIZADA, pid, estado_fs);
                log_info(logger, "PID: <%d> - Operacion: <IO_FS_WRITE> - Finalizada \n", pid);
                liberar_lista_de_peticiones_memoria(direcciones);
                free(nombre_archivo);
                break;
            }
            case SOLICITUD_IO_FS_READ:
            {
                uint32_t pid, bytes, puntero;
                char* nombre_archivo;
                recibir_generico_triple_entero_y_string(conexion_kernel, &pid, &bytes, &puntero, &nombre_archivo);
                
                ignorar_op_code(conexion_kernel);
                t_list* direcciones = recibir_lista_peticiones_memoria(conexion_kernel);

                log_info(logger, "PID: <%u> - Operacion: <IO_FS_READ> - Iniciada", pid);

                log_info(logger, "PID: <%u> - Leer Archivo: <%s> - Tamanio a Leer: <%u> - Puntero Archivo: <%u>", pid, nombre_archivo, bytes, puntero);
                estado_interfaz estado_fs = leer_archivo_dial(nombre_archivo, bytes, puntero, direcciones, pid);
                if(estado_fs == TODO_OK)log_info(logger, "PID: <%u> - Leer Archivo: <%s> TODO_OK", pid, nombre_archivo);
                if(estado_fs == PUNTERO_INVALIDO)log_info(logger, "PID: <%u> - Leer Archivo: <%s> PUNTERO_INVALIDO", pid, nombre_archivo);
                if(estado_fs == ARCHIVO_NO_EXISTE)log_info(logger, "PID: <%u> - Leer Archivo: <%s> ARCHIVO_NO_EXISTE", pid, nombre_archivo);
                if(estado_fs == LECTURA_INVALIDA)log_info(logger, "PID: <%u> - Leer Archivo: <%s> LECTURA_INVALIDA", pid, nombre_archivo);

                enviar_generico_doble_entero(conexion_kernel, SOLICITUD_IO_FS_READ_FINALIZADA, pid, estado_fs);
                log_info(logger, "PID: <%d> - Operacion: <IO_FS_READ> - Finalizada \n", pid);
                liberar_lista_de_peticiones_memoria(direcciones);
                free(nombre_archivo);
                break;
            }
            case -1:
                log_error(logger, "el server %s cerro la conexion", nombre_modulo_server);
                return; // finalizando hilo
            default:
                log_error(logger, "El codigo de operacion %u es incorrecto - %s", cod_op, nombre_modulo_server);
                return; // finalizando hilo
        }
    }

}