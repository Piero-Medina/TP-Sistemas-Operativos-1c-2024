#ifndef ENUM_H
#define ENUM_H

// aca declararemos los diferentes codigos de operacion
typedef enum{
    HANDSHAKE,                 // (hacia cualquier modulo) - procesar handshake 

    NUEVO_PROCESO_MEMORIA,     // de kernel -> memoria - avisar sobre la creacion de un proceso
    EJECUTAR_PROCESO,          // de kernel -> CPU - manda contexto para ser ejecutado en cpu

    WAIT,                      // cpu -> kernel - procesar un wait sobre los recursos 
    SIGNAL,                    // cpu -> kernel - procesar un signal sobre los recursos
    PETICION_IO,               // cpu -> kernel - procesar una peticion de IO  
    PROCESO_FINALIZADO         // cpu -> kernel - procesar la finalizacion de proceso (exit)
}op_code;

#endif 