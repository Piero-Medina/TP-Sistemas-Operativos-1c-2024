#include <comunicacion/comunicacion.h>
///////////////////////////////////////// WRAPPERS ////////////////////////////////////////////// 
void avisar_nuevo_proceso_memoria(int conexion_memoria, int pid, char* path_intrucciones){
    envio_generico_entero_y_string(conexion_memoria, NUEVO_PROCESO_MEMORIA, pid, path_intrucciones);
}

void avisar_desalojo_a_cpu(int conexion_cpu, int op_code, char* motivo){
    envio_generico_string(conexion_cpu, op_code, motivo);
}

void solicitar_intruccion_a_memoria(int conexion, uint8_t op_code, uint32_t pid, uint32_t pc){
    envio_generico_doble_entero(conexion, op_code, pid, pc);
}

/////////////////////////////////////////////////////////////////////////////////////// 
void envio_generico_entero_y_string(int conexion, uint8_t op_code, uint32_t entero, char* string){
    uint32_t length = strlen(string) + 1;
    uint32_t size_buffer = sizeof(uint32_t) + sizeof(uint32_t) + length; 
    
    t_paquete* paquete = paquete_create_with_buffer_size(size_buffer, op_code);

    buffer_add_uint32(paquete->buffer, entero);
    buffer_add_string(paquete->buffer, length, string);

    uint32_t size_a_enviar = 0;
    void* a_enviar = serializar_paquete(paquete, &size_a_enviar);

    send(conexion, a_enviar, size_a_enviar, 0);

    paquete_detroy(paquete);
    free(a_enviar);
}

void recibir_generico_entero_string(int conexion, uint32_t* entero, char** string){
    t_buffer* buffer = recibir_buffer(conexion);

    *entero = buffer_read_uint32(buffer);
    *string = buffer_read_string(buffer);

    buffer_destroy(buffer);
}

/////////////////////////////////////////////////////////////////////////////////////// 
void envio_generico_string(int conexion, uint8_t op_code, char* string){
    uint32_t length = strlen(string) + 1;
    uint32_t size_buffer = sizeof(uint32_t) + length; 
    
    t_paquete* paquete = paquete_create_with_buffer_size(size_buffer, op_code);

    buffer_add_string(paquete->buffer, length, string);

    uint32_t size_a_enviar = 0;
    void* a_enviar = serializar_paquete(paquete, &size_a_enviar);

    send(conexion, a_enviar, size_a_enviar, 0);

    paquete_detroy(paquete);
    free(a_enviar);
}

char* recibir_generico_string(int conexion){
    t_buffer* buffer = recibir_buffer(conexion);
    
    char* string = buffer_read_string(buffer);

    buffer_destroy(buffer);

    return string;
}

///////////////////////////////////////////////////////////////////////////////////////

void envio_generico_entero(int conexion, uint8_t op_code, uint32_t entero){
    uint32_t size_buffer = sizeof(uint32_t); 
    
    t_paquete* paquete = paquete_create_with_buffer_size(size_buffer, op_code);

    buffer_add_uint32(paquete->buffer, entero);

    uint32_t size_a_enviar = 0;
    void* a_enviar = serializar_paquete(paquete, &size_a_enviar);

    send(conexion, a_enviar, size_a_enviar, 0);

    paquete_detroy(paquete);
    free(a_enviar);
}

uint32_t recibo_generico_entero(int conexion){
    t_buffer* buffer = recibir_buffer(conexion);
    
    uint32_t entero = buffer_read_uint32(buffer);

    buffer_destroy(buffer);

    return entero;
}

///////////////////////////////////////////////////////////////////////////////////////

void envio_generico_int32(int conexion, uint8_t op_code, int32_t entero){
    uint32_t size_buffer = sizeof(int32_t); 
    
    t_paquete* paquete = paquete_create_with_buffer_size(size_buffer, op_code);

    buffer_add_int32(paquete->buffer, entero);

    uint32_t size_a_enviar = 0;
    void* a_enviar = serializar_paquete(paquete, &size_a_enviar);

    send(conexion, a_enviar, size_a_enviar, 0);

    paquete_detroy(paquete);
    free(a_enviar);
}

int32_t recibo_generico_int32(int conexion){
    t_buffer* buffer = recibir_buffer(conexion);
    
    int32_t entero = buffer_read_int32(buffer);

    buffer_destroy(buffer);

    return entero;
}

///////////////////////////////////////////////////////////////////////////////////////

void envio_generico_doble_entero(int conexion, uint8_t op_code, uint32_t entero1, uint32_t entero2){
    uint32_t size_buffer = sizeof(uint32_t) * 2; 
    
    t_paquete* paquete = paquete_create_with_buffer_size(size_buffer, op_code);

    buffer_add_uint32(paquete->buffer, entero1);
    buffer_add_uint32(paquete->buffer, entero2);

    uint32_t size_a_enviar = 0;
    void* a_enviar = serializar_paquete(paquete, &size_a_enviar);

    send(conexion, a_enviar, size_a_enviar, 0);

    paquete_detroy(paquete);
    free(a_enviar);
}

void recibo_generico_doble_entero(int conexion, uint32_t* entero1, uint32_t* entero2){
    t_buffer* buffer = recibir_buffer(conexion);
    
    *entero1 = buffer_read_uint32(buffer);
    *entero2 = buffer_read_uint32(buffer);

    buffer_destroy(buffer);
}

///////////////////////////////////////////////////////////////////////////////////////

void envio_generico_op_code(int conexion, uint8_t op_code){
    send(conexion, &op_code, sizeof(uint8_t), 0);
}

int recibo_generico_op_code(int conexion){
    uint8_t tmp;
    recv(conexion, &tmp, sizeof(uint8_t), MSG_WAITALL);
    return (int) tmp;
}

void validar_respuesta_op_code(int conexion, uint8_t op_code_esperado, t_log* logger){
    int respuesta = recibo_generico_op_code(conexion);
    if(respuesta == op_code_esperado) log_info(logger, "Respuesta OK");
    if(respuesta != op_code_esperado) log_info(logger, "Respuesta Fallida \n");
}

void ignorar_op_code(int conexion){
    /*
        La siguiente directiva del preprocesador se utiliza para indicar al compilador
        que la variable 'tmp' puede estar sin usar, evitando así que se emita un warning.
    */
    int tmp __attribute__((unused)) = recibo_generico_op_code(conexion);
    //int tmp = recibo_generico_op_code(conexion);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void enviar_generico_doble_entero_y_string(int conexion, uint8_t op_code, uint32_t entero1, uint32_t entero2, char* string){
    uint32_t length = strlen(string) + 1;
    uint32_t size_buffer = sizeof(uint32_t) * 2 +
                           sizeof(uint32_t) + length; 
    
    t_paquete* paquete = paquete_create_with_buffer_size(size_buffer, op_code);

    buffer_add_uint32(paquete->buffer, entero1);
    buffer_add_uint32(paquete->buffer, entero2);
    buffer_add_string(paquete->buffer, length, string);

    uint32_t size_a_enviar = 0;
    void* a_enviar = serializar_paquete(paquete, &size_a_enviar);

    send(conexion, a_enviar, size_a_enviar, 0);

    paquete_detroy(paquete);
    free(a_enviar);
}

void recibir_generico_doble_entero_y_string(int conexion, uint32_t* entero1, uint32_t* entero2, char** string){
    t_buffer* buffer = recibir_buffer(conexion);
    
    *entero1 = buffer_read_uint32(buffer);
    *entero2 = buffer_read_uint32(buffer);
    *string = buffer_read_string(buffer);

    buffer_destroy(buffer);
}

///////////////////////////////////////////////////////////////////////////////////////

void enviar_data(int conexion, uint8_t op_code, void* data, uint32_t bytes){
    uint32_t size_a_enviar = sizeof(uint8_t) + sizeof(uint32_t) + bytes;

    void* a_enviar = malloc(size_a_enviar);

    uint32_t offset = 0;
    memcpy(a_enviar + offset, &op_code, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(a_enviar + offset, &bytes, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, data, bytes);

    send(conexion, a_enviar, size_a_enviar, 0);

    free(a_enviar);
}

void* recibir_data(int conexion, uint32_t* bytes_recibidos) {
    uint32_t bytes = 0;
    recv(conexion, &bytes, sizeof(uint32_t), MSG_WAITALL);
    
    void* data = malloc(bytes);
    recv(conexion, data, bytes, MSG_WAITALL);

    if (bytes_recibidos != NULL) {
        *bytes_recibidos = bytes;
    }

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////

void enviar_lista_peticiones_memoria(int conexion, uint8_t codigo_operacion, t_list* lista){
    t_paquete* paquete = paquete_create_with_buffer_null(codigo_operacion);

    paquete->buffer = serializar_lista_de_t_peticion_memoria(lista);

    uint32_t size_a_enviar = 0;
    void* a_enviar = serializar_paquete(paquete, &size_a_enviar);
    send(conexion, a_enviar, size_a_enviar, 0);
    
    paquete_detroy(paquete);
    free(a_enviar);

}

t_list* recibir_lista_peticiones_memoria(int conexion){
    t_buffer* buffer = recibir_buffer(conexion);

    t_list* tmp = deserializar_lista_de_t_peticion_memoria(buffer);

    buffer_destroy(buffer);

    return tmp;
}

///////////////////////////////////////////////////////////////////////////////////////

void enviar_pcb(int conexion, t_PCB* pcb, uint8_t codigo_operacion){
    t_paquete* paquete = paquete_create_with_buffer_null(codigo_operacion);
    paquete->buffer = serializar_pcb(pcb);

    uint32_t size_a_enviar = 0;
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
    uint32_t size = sizeof(uint32_t) * 2 +  //(pid, quamtum)
                    sizeof(uint8_t) * 1 +   //(estado)
                    sizeof(uint32_t) * 8 +  
                    sizeof(uint8_t) * 4;

    t_buffer* buffer = buffer_create(size);

    buffer_add_uint32(buffer, pcb->pid);
    buffer_add_uint32(buffer, pcb->program_counter);
    buffer_add_uint32(buffer, pcb->quantum);

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

    buffer_add_uint8(buffer, pcb->estado);

    return buffer;
}

t_PCB* deserializar_pcb(t_buffer* buffer){
    t_PCB* pcb = malloc(sizeof(t_PCB));
    pcb->registros = malloc(sizeof(registros_cpu));

    pcb->pid = buffer_read_uint32(buffer);
    pcb->program_counter = buffer_read_uint32(buffer);
    pcb->quantum = buffer_read_uint32(buffer);

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

    pcb->estado = (estado_pcb) buffer_read_uint8(buffer); // se castea automaticamente

    return pcb;
}
//////////////////////////////////////////////////////////////////////////////////////////
void enviar_instruccion(int conexion, t_instruccion* instruccion, uint8_t codigo_operacion){
    t_paquete* paquete = paquete_create_with_buffer_null(codigo_operacion);
    paquete->buffer = serializar_instruccion(instruccion);

    uint32_t size_a_enviar = 0;
    void* a_enviar = serializar_paquete(paquete, &size_a_enviar);
    send(conexion, a_enviar, size_a_enviar, 0);
    
    paquete_detroy(paquete);
    free(a_enviar);
}

t_instruccion* recibir_instruccion(int conexion){
    t_buffer* buffer = recibir_buffer(conexion);

    t_instruccion* tmp = deserializar_instruccion(buffer);

    buffer_destroy(buffer);

    return tmp;
}

t_buffer* serializar_instruccion(t_instruccion* instruccion){
    uint32_t largo_lista = (uint32_t) list_size(instruccion->parametros);
    int size_lista_serializable = tamanio_serializable_lista_de_string(instruccion->parametros);

    uint32_t size = sizeof(uint8_t) +                 // identificador
                    sizeof(uint32_t) +                // largo de la lista 
                    size_lista_serializable;          // largo total de elementos de la lista

    t_buffer* buffer = buffer_create(size);

    buffer_add_uint8(buffer, instruccion->identificador);
    buffer_add_list_string(buffer, instruccion->parametros, largo_lista);

    return buffer;
}

t_instruccion* deserializar_instruccion(t_buffer* buffer){
    t_instruccion* tmp = malloc(sizeof(t_instruccion));

    tmp->identificador = (t_identificador) buffer_read_uint8(buffer);
    tmp->parametros = buffer_read_list_string(buffer);

    return tmp;
}

///////////////////////////////////////////////////////////////////
t_buffer* serializar_lista_de_string(t_list* lista_de_string){
    uint32_t largo_lista = (uint32_t) list_size(lista_de_string);
    int size_lista_serializable = tamanio_serializable_lista_de_string(lista_de_string);

    uint32_t size = sizeof(uint32_t) +                // largo de la lista 
                    size_lista_serializable;          // largo total de elementos de la lista

    t_buffer* buffer = buffer_create(size);

    buffer_add_list_string(buffer, lista_de_string, largo_lista);
    
    return buffer;
}

t_list* deserializar_lista_de_string(t_buffer* buffer){
    return buffer_read_list_string(buffer);
}

int tamanio_serializable_lista_de_string(t_list* lista_de_string){
    int size = 0;
    char* string = NULL;
    uint32_t length_string = 0;

    for (int i = 0; i < lista_de_string->elements_count; i++){
        string = (char*) list_get(lista_de_string, i); // devuelve solo la direccion de memoria del elemento, no la remueve de la lista
        length_string = strlen(string) + 1;
        size += sizeof(uint32_t) + length_string; 
    }
    return size;
}

void buffer_add_list_string(t_buffer* buffer, t_list* lista_de_string, uint32_t size_lista){
    char* string = NULL;
    uint32_t length_string = 0;

    // agrega al buffer el largo de la lista
    buffer_add_uint32(buffer, size_lista);

    // agrega cada string de la lista al buffer
    for (int i = 0; i < size_lista; i++){
        string = (char*) list_get(lista_de_string, i);
        length_string = strlen(string) + 1;
        buffer_add_string(buffer, length_string, string); 
    }
}

t_list* buffer_read_list_string(t_buffer* buffer){
    t_list* tmp = list_create();

    // lee del buffer el largo de la lista
    uint32_t size_lista = buffer_read_uint32(buffer);

    // lee cada string del buffer y lo agrega a la lista
    for(int i = 0; i < size_lista; i++){
        list_add(tmp, (void*) buffer_read_string(buffer));
    }
    return tmp;
}

//////////////////////////////////////////////////////////////////////////////////////

