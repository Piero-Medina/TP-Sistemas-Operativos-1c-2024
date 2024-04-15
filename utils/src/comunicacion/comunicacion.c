#include <comunicacion/comunicacion.h>

void avisar_nuevo_proceso_memoria(int conexion_memoria, int PID, char* path_intrucciones){
    uint32_t length = strlen(path_intrucciones) + 1;
    uint32_t size_buffer = sizeof(int) + sizeof(uint32_t) + length; 
    
    t_paquete* paquete = paquete_create_with_buffer_size(size_buffer, NUEVO_PROCESO_MEMORIA);

    buffer_add_int(paquete->buffer, PID);
    buffer_add_string(paquete->buffer, length, path_intrucciones);

    size_t size_a_enviar = 0;
    void* a_enviar = serializar_paquete(paquete, &size_a_enviar);

    send(conexion_memoria, a_enviar, size_a_enviar, 0);

    paquete_detroy(paquete);
    free(a_enviar);
}