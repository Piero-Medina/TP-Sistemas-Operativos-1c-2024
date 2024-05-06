#include "manejo_interfaz.h"


void agregar_interfaz(t_dictionary* interfaces, char* nombre_interfaz, int conexion, int tipo_interfaz){
    t_interfaz* tmp = malloc(sizeof(t_interfaz));
    
    tmp->socket = conexion;
    tmp->tipo = tipo_interfaz;
    tmp->ocupado = false;

    dictionary_put(interfaces, nombre_interfaz, (void*) tmp);
}

void quitar_interfaz(t_dictionary* interfaces, char* nombre_interfaz){
    dictionary_remove_and_destroy(interfaces, nombre_interfaz, liberar_elemento_interfaz);
}

void verificar_tipo_interfaz(int conexion, int tipo_de_interfaz, char* nombre_interfaz){
    int operacion = recibo_generico_op_code(conexion);

    switch (tipo_de_interfaz){
        case GENERICA:
            verificar_operacion_generica(conexion, nombre_interfaz, operacion);
            break;
        case STDIN:
            break;
        case STDOUT:
            break;
        case DIALFS:
            break;
    }
}

void verificar_operacion_generica(int conexion, char* nombre_interfaz, int operacion){
    if(operacion == IO_GEN_SLEEP){
        //int unidades_genericas = recibo_generico_entero(conexion);
        //sem_post(&mutex_conexion_cpu_dispatch); // terminamos de recibir todos los datos
        sem_post(&mutex_conexion_cpu_dispatch);

        // validar que exista el nombre de interfaz
        // validar que admite la operacion

        // procesar_io_generica()
        // sem_post(hay_peticion_io_por_procesar);

        // recien validamos aca para no cortar el envio de la cpu
        // recibimos todo y dejamos al socket libre para que trabaje
    }
}

void verificar_operacion_stdin(int conexion, char* nombre_interfaz, int operacion){
    if(operacion == IO_STDIN_READ){
        // validar que exista el nombre de interfaz
        // validar que admite la operacion

        // recien validamos aca para no cortar el envio de la cpu
        // recibimos todo y dejamos al socket libre para que trabaje
    }
}

void verificar_operacion_stdout(int conexion, char* nombre_interfaz, int operacion){
    if(operacion == IO_STDIN_READ){
        // validar que exista el nombre de interfaz
        // validar que admite la operacion

        // recien validamos aca para no cortar el envio de la cpu
        // recibimos todo y dejamos al socket libre para que trabaje
    }
}

void verificar_operacion_dialFs(int conexion, char* nombre_interfaz, int operacion){
    switch (operacion){
        case IO_FS_CREATE:
            // validar que exista el nombre de interfaz
            // validar que admite la operacion

            // recien validamos aca para no cortar el envio de la cpu
             // recibimos todo y dejamos al socket libre para que trabaje
            break;
        case IO_FS_DELETE:
            //  
            break;
        case IO_FS_TRUNCATE:
            // 
            break;
        case IO_FS_WRITE:
            // 
            break;
        case IO_FS_READ:
            // 
            break;
    }
}