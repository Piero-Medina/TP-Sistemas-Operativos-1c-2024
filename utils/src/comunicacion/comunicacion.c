#include <comunicacion/comunicacion.h>
//
void avisar_nuevo_proceso_memoria(int conexion_memoria, int pid, char* path_intrucciones){
    envio_generico_entero_y_string(conexion_memoria, NUEVO_PROCESO_MEMORIA, pid, path_intrucciones);
}

void avisar_desalojo_a_cpu(int conexion_cpu, int op_code, char* motivo){
    envio_generico_string(conexion_cpu, op_code, motivo);
}

/////////////////////////////////////////////////////////////////////////////////////// 
void envio_generico_entero_y_string(int conexion, int op_code, int entero, char* string){
    uint32_t length = strlen(string) + 1;
    uint32_t size_buffer = sizeof(int) + sizeof(uint32_t) + length; 
    
    t_paquete* paquete = paquete_create_with_buffer_size(size_buffer, op_code);

    buffer_add_int(paquete->buffer, entero);
    buffer_add_string(paquete->buffer, length, string);

    size_t size_a_enviar = 0;
    void* a_enviar = serializar_paquete(paquete, &size_a_enviar);

    send(conexion, a_enviar, size_a_enviar, 0);

    paquete_detroy(paquete);
    free(a_enviar);
}

void recibir_generico_entero_string(int conexion, int* entero, char** string){
    t_buffer* buffer = recibir_buffer(conexion);

    *entero = buffer_read_int(buffer);
    *string = buffer_read_string(buffer);

    buffer_destroy(buffer);
}

/////////////////////////////////////////////////////////////////////////////////////// 
void envio_generico_string(int conexion, int op_code, char* string){
    uint32_t length = strlen(string) + 1;
    uint32_t size_buffer = sizeof(uint32_t) + length; 
    
    t_paquete* paquete = paquete_create_with_buffer_size(size_buffer, op_code);

    buffer_add_string(paquete->buffer, length, string);

    size_t size_a_enviar = 0;
    void* a_enviar = serializar_paquete(paquete, &size_a_enviar);

    send(conexion, a_enviar, size_a_enviar, 0);

    paquete_detroy(paquete);
    free(a_enviar);
}

void recibir_generico_string(int conexion, char** string){
    t_buffer* buffer = recibir_buffer(conexion);
    
    *string = buffer_read_string(buffer);

    buffer_destroy(buffer);
}

///////////////////////////////////////////////////////////////////////////////////////
void envio_generico_op_code(int conexion, int op_code){
    send(conexion, &op_code, sizeof(int), 0);
}

int recibo_generico_op_code(int conexion){
    int tmp;
    recv(conexion, &tmp, sizeof(int), MSG_WAITALL);
    return tmp;
}

void validar_respuesta_op_code(int conexion, int op_code_esperado, t_log* logger){
    int respuesta = recibo_generico_op_code(conexion);
    if(respuesta == op_code_esperado) log_info(logger, "Respuesta OK");
    else log_info(logger, "Respuesta Fallida");
}

///////////////////////////////////////////////////////////////////////////////////////
void enviar_pcb(int conexion, t_PCB* pcb, int codigo_operacion){
    t_paquete* paquete = paquete_create_with_buffer_null(codigo_operacion);
    paquete->buffer = serializar_pcb(pcb);

    size_t size_a_enviar = 0;
    void* a_enviar = serializar_paquete(paquete, &size_a_enviar);
    send(conexion, a_enviar, size_a_enviar, 0);
    
    paquete_detroy(paquete);
    free(a_enviar);
}

t_PCB* recibir_pcb(int conexion){
    t_buffer* buffer = malloc(sizeof(t_buffer));

    recv(conexion, &buffer->size, sizeof(uint32_t), MSG_WAITALL);
    buffer->stream = malloc(buffer->size);
    recv(conexion, buffer->stream, buffer->size, MSG_WAITALL);

    buffer->offset = 0;
    t_PCB* tmp = deserializar_pcb(buffer);

    buffer_destroy(buffer);

    return tmp;
}

t_buffer* serializar_pcb(t_PCB* pcb){
    uint32_t size = sizeof(int) * 3 + 
                    sizeof(uint32_t) * 8 +
                    sizeof(uint8_t) * 4;

    t_buffer* buffer = buffer_create(size);

    buffer_add_int(buffer, pcb->pid);
    buffer_add_uint32(buffer, pcb->program_counter);
    buffer_add_int(buffer, pcb->quantum);

    buffer_add_uint32(buffer, pcb->registros->PC);
    buffer_add_uint8(buffer, pcb->registros->AX);
    buffer_add_uint8(buffer, pcb->registros->BX);
    buffer_add_uint8(buffer, pcb->registros->CX);
    buffer_add_uint8(buffer, pcb->registros->DX);
    buffer_add_uint32(buffer, pcb->registros->EAX);
    buffer_add_uint32(buffer, pcb->registros->EBX);
    buffer_add_uint32(buffer, pcb->registros->ECX);
    buffer_add_uint32(buffer, pcb->registros->EDX);
    buffer_add_uint32(buffer, pcb->registros->SI);
    buffer_add_uint32(buffer, pcb->registros->DI);

    buffer_add_int(buffer, pcb->estado);

    return buffer;
}

t_PCB* deserializar_pcb(t_buffer* buffer){
    t_PCB* pcb = malloc(sizeof(t_PCB));
    pcb->registros = malloc(sizeof(registros_cpu));

    pcb->pid = buffer_read_int(buffer);
    pcb->program_counter = buffer_read_uint32(buffer);
    pcb->quantum = buffer_read_int(buffer);

    pcb->registros->PC = buffer_read_uint32(buffer);
    pcb->registros->AX = buffer_read_uint8(buffer);
    pcb->registros->BX = buffer_read_uint8(buffer);
    pcb->registros->CX = buffer_read_uint8(buffer);
    pcb->registros->DX = buffer_read_uint8(buffer);
    pcb->registros->EAX = buffer_read_uint32(buffer);
    pcb->registros->EBX = buffer_read_uint32(buffer);
    pcb->registros->ECX = buffer_read_uint32(buffer);
    pcb->registros->EDX = buffer_read_uint32(buffer);
    pcb->registros->SI = buffer_read_uint32(buffer);
    pcb->registros->DI = buffer_read_uint32(buffer);

    pcb->estado = buffer_read_int(buffer);

    return pcb;
}

