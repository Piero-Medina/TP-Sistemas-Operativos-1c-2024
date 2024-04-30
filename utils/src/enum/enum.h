#ifndef ENUM_H
#define ENUM_H

// aca declararemos los diferentes codigos de operacion
typedef enum{
    HANDSHAKE,                      // (hacia cualquier modulo) - procesar handshake
    REGISTRO_INTERFAZ,              // entradaSalida -> kernel - avisa al kernel para que la interfaz quede registrada

    KERNEL_OK,                      // kernel -> cualquier modulo (avisa que la operacion se completo)
    MEMORIA_OK,                     // memoria -> cualquier modulo (avisa que la operacion se completo)  

    NUEVO_PROCESO_MEMORIA,          // de kernel -> memoria - avisar sobre la creacion de un proceso
    PROCESO_FINALIZADO_MEMORIA,     // de kernel -> memoria - avisar sobre la eliminacion de un proceso
    EJECUTAR_PROCESO,               // de kernel -> CPU - manda contexto para ser ejecutado en cpu

    DESALOJO,                       // cpu <--> kernel - procesar un desalojo de proceso
    WAIT_KERNEL,                    // cpu -> kernel - procesar un wait sobre los recursos 
    SIGNAL_KERNEL,                  // cpu -> kernel - procesar un signal sobre los recursos
    PETICION_IO,                    // cpu -> kernel - procesar una peticion de IO  
    PROCESO_FINALIZADO,             // cpu -> kernel - procesar la finalizacion de proceso (exit)
    SOLICITAR_INTRUCCION_MEMORIA    // cpu -> memoria - pedir una intruccion a Memoria
}op_code;


typedef enum{
    GENERICA,
    STDIN,
    STDOUT,
    DIALFS
}tipo_interfaz;

#endif 