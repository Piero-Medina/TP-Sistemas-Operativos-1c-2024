#ifndef ENUM_H
#define ENUM_H

// aca declararemos los diferentes codigos de operacion
typedef enum{
    HANDSHAKE,                         // (hacia cualquier modulo) - procesar handshake
    REGISTRO_INTERFAZ,                 // entradaSalida -> kernel - avisa al kernel para que la interfaz quede registrada

    KERNEL_OK,                         // kernel -> cualquier modulo (avisa que la operacion se completo)
    MEMORIA_OK,                        // memoria -> cualquier modulo (avisa que la operacion se completo)  

    SOLICITUD_TAMANIO_PAGINA,          // de cpu -> memoria - solicitud de tamanio de pagina que usa memoria
    NUEVO_PROCESO_MEMORIA,             // de kernel -> memoria - avisar sobre la creacion de un proceso
    PROCESO_FINALIZADO_MEMORIA,        // de kernel -> memoria - avisar sobre la eliminacion de un proceso
    EJECUTAR_PROCESO,                  // de kernel -> CPU - manda contexto para ser ejecutado en cpu

    DESALOJO,                          // cpu <--> kernel - procesar un desalojo de proceso
    WAIT_KERNEL,                       // cpu -> kernel - procesar un wait sobre los recursos 
    SIGNAL_KERNEL,                     // cpu -> kernel - procesar un signal sobre los recursos
    PETICION_IO,                       // cpu -> kernel - procesar una peticion de IO  
    PROCESO_FINALIZADO,                // cpu -> kernel - procesar la finalizacion de proceso (exit)
    SOLICITAR_INTRUCCION_MEMORIA,      // cpu -> memoria - pedir una intruccion a Memoria

    IGNORAR_OP_CODE,                   // para mensajes lo cuales no es necesario leer el op_code

    MMU_OK,
    OUT_OF_MEMORY,
    SEGMENTATION_FAULT,
    SOLICITUD_MARCO_MEMORIA,
    SOLICITUD_LECTURA_MEMORIA,
    SOLICITUD_ESCRITURA_MEMORIA,

    SOLICITUD_IO_GEN_SLEEP,            
    SOLICITUD_IO_GEN_SLEEP_FINALIZADA,

    SOLICITUD_IO_FS_CREATE,
    SOLICITUD_IO_FS_CREATE_FINALIZADA,
    SOLICITUD_IO_FS_DELETE,
    SOLICITUD_IO_FS_DELETE_FINALIZADA,
    SOLICITUD_IO_FS_TRUNCATE,
    SOLICITUD_IO_FS_TRUNCATE_FINALIZADA,
    SOLICITUD_IO_FS_WRITE,
    SOLICITUD_IO_FS_WRITE_FINALIZADA,
    SOLICITUD_IO_FS_READ,
    SOLICITUD_IO_FS_READ_FINALIZADA
}op_code;


typedef enum{
    GENERICA,
    STDIN,
    STDOUT,
    DIALFS
}tipo_interfaz;

typedef enum{
    TODO_OK,
    NO_PROCESAR,
    SIN_ESPACIO,
    LECTURA_INVALIDA,
    ESCRITURA_INVALIDA,
    ARCHIVO_NO_EXISTE
}estado_interfaz;

#endif 