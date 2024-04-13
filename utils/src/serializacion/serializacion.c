#include <serializacion/serializacion.h>

t_paquete* paquete_create_add_buffer(t_buffer* buffer, int codigo_operacion){
    t_paquete* paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = codigo_operacion;
    paquete->buffer = buffer;

    return paquete;
}

t_paquete* paquete_create_with_buffer_null(int codigo_operacion){
    t_paquete* paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = codigo_operacion;
    paquete->buffer = NULL;

    return paquete;
}

t_buffer* buffer_create(uint32_t size){
    t_buffer* buffer = malloc(sizeof(t_buffer));

    buffer->offset = 0;
    buffer->size = size;
    buffer->stream = malloc(buffer->size);

    return buffer;
}

void buffer_add(t_buffer* buffer, void* data, uint32_t size){
    memcpy(buffer->stream + buffer->offset, data, size);
    buffer->offset += size;
}

void buffer_read(t_buffer* buffer, void* data, uint32_t size){
    memcpy(data, buffer->stream + buffer->offset, size);
    buffer->offset += size;
}

void paquete_detroy(t_paquete* paquete){
    buffer_destroy(paquete->buffer);
	free(paquete);
}

void buffer_destroy(t_buffer* buffer){
    free(buffer->stream);
    free(buffer);
}


// --------------------------  agregar tipos de dato a un buffer -----------------------
 
 void buffer_add_int(t_buffer* buffer, int data){
    buffer_add(buffer, (void *) &data, sizeof(int));
 }

 void buffer_add_uint32(t_buffer* buffer, uint32_t data){
    buffer_add(buffer, (void *) &data, sizeof(uint32_t ));
 }

 void buffer_add_uint8(t_buffer* buffer, uint8_t data){
    buffer_add(buffer, (void *) &data, sizeof(uint8_t ));
 }

 void buffer_add_string(t_buffer* buffer, uint32_t length, char* string){
    buffer_add_uint32(buffer, length); // agregamos al buffer la longitud del string
    buffer_add(buffer, (void *) string, length); // agregamos al buffer el propio string
 }

// --------------------------  leer tipos de dato de un buffer -----------------------

 int buffer_read_int(t_buffer *buffer){
    int tmp;
    buffer_read(buffer, (void*) &tmp, sizeof(int));
    return tmp;
 }

 uint32_t buffer_read_uint32(t_buffer *buffer){
    uint32_t tmp;
    buffer_read(buffer, (void*) &tmp, sizeof(uint32_t));
    return tmp;
 }

 uint8_t buffer_read_uint8(t_buffer *buffer){
    uint8_t  tmp;
    buffer_read(buffer, (void*) &tmp, sizeof(uint8_t));
    return tmp;
 }

 char* buffer_read_string(t_buffer* buffer){
    char* tmp = NULL;
    uint32_t size = buffer_read_uint32(buffer); 
    tmp = malloc(size);
    buffer_read(buffer, (void*) tmp, size);
    return tmp;
 }


//-------------------------------------- SERIALIZACION DE PAQUETE -------------------------------
 void* serializar_paquete(t_paquete* paquete, size_t* size_a_enviar){

    *size_a_enviar = sizeof(int) +          // codigo de operacion
                     sizeof(uint32_t) +     // tamaño del buffer
                     paquete->buffer->size; // el propio stream del buffer

    void* a_enviar = malloc(*size_a_enviar);

    int offset = 0;
    memcpy(a_enviar + offset, &paquete->codigo_operacion, sizeof(int));
    offset += sizeof(int);
    memcpy(a_enviar + offset, &paquete->buffer->size, sizeof(uint32_t)); 
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

    return a_enviar;
 }

//-------------------------------------- HANDSHAKE ---------------------------------------------- 
 void enviar_handshake(int conexion, int codigo_operacion, char* modulo_origen, char* modulo_destino, t_log* logger){
    t_paquete* paquete = paquete_create_with_buffer_null(codigo_operacion);
    
    uint32_t length = strlen(modulo_origen) + 1;
    uint32_t size_buffer = sizeof(uint32_t) + length;

    t_buffer* buffer = buffer_create(size_buffer);

    buffer_add_string(buffer, length, modulo_origen);

    paquete->buffer = buffer;

    size_t size_a_enviar = 0;
    void* a_enviar = serializar_paquete(paquete, &size_a_enviar);

    send(conexion, a_enviar, size_a_enviar, 0);

    paquete_detroy(paquete);

    free(a_enviar);

    estado_handshake(conexion, modulo_destino, logger);
 }

 char* recibir_handshake(int fd_cliente){
    t_buffer* buffer = malloc(sizeof(t_buffer));
    
    recv(fd_cliente, &buffer->size, sizeof(uint32_t), MSG_WAITALL);
    buffer->stream = malloc(buffer->size);
    recv(fd_cliente, buffer->stream, buffer->size, MSG_WAITALL);

    buffer->offset = 0;

    char* modulo = buffer_read_string(buffer); // liberar

    buffer_destroy(buffer);

    return modulo;
 }

 void estado_handshake(int conexion, char* modulo, t_log* logger){
    int valor = -1;
    recv(conexion, &valor, sizeof(int), MSG_WAITALL);
    if(valor == 0) log_info (logger, "Handshake Aceptado por Modulo: %s",modulo);
    else log_info (logger, "Handshake Denegado por Modulo: %s",modulo);
 }

 void responder_handshake(int fd_cliente){
    int valor = 0;
    send(fd_cliente, &valor, sizeof(int), 0);
 }

//-------------------------------------- EJEMPLO -------------------------------------------------

void ver_persona(t_persona* persona) {
    printf("DNI: %u\n", persona->dni);
    printf("Edad: %u\n", persona->edad);
    printf("Pasaporte: %u\n", persona->pasaporte);
    printf("Nombre: %s\n", persona->nombre);
}

t_persona* crear_persona(uint32_t dni, uint8_t edad, uint32_t pasaporte, const char* nombre) {
    t_persona* nueva_persona = malloc(sizeof(t_persona));

    nueva_persona->dni = dni;
    nueva_persona->edad = edad;
    nueva_persona->pasaporte = pasaporte;

    // Asignar memoria para el nombre y copiar la cadena
    nueva_persona->nombre = malloc(strlen(nombre) + 1);

    strcpy(nueva_persona->nombre, nombre);

    return nueva_persona;
}

void liberar_persona(t_persona* persona) {
    free(persona->nombre);
    free(persona);
}

t_buffer *serializar_persona(t_persona *persona){
    uint32_t length_nombre = strlen(persona->nombre) + 1; 
    uint32_t size = sizeof(uint32_t) * 2 +               // DNI y Pasaporte
                    sizeof(uint8_t) +                    // Edad 
                    sizeof(uint32_t) + length_nombre;    // Longitud del nombre, y el propio nombre

    t_buffer* buffer = buffer_create(size);

    buffer_add_uint32(buffer, persona->dni);
    buffer_add_uint8(buffer, persona->edad);
    buffer_add_uint32(buffer, persona->pasaporte);
    buffer_add_string(buffer, length_nombre, persona->nombre);

    return buffer;
}

t_persona* deserializar_persona(t_buffer *buffer) {
    t_persona *persona = malloc(sizeof(t_persona));

    persona->dni = buffer_read_uint32(buffer);
    persona->edad = buffer_read_uint8(buffer);
    persona->pasaporte = buffer_read_uint32(buffer);
    persona->nombre = buffer_read_string(buffer);

    return persona;
}


void enviar_persona(int socket, t_persona* persona, int codigo_operacion){
    t_paquete* paquete = paquete_create_with_buffer_null(codigo_operacion);

    paquete->buffer = serializar_persona(persona);

    size_t size_a_enviar = 0;
    void* a_enviar = serializar_paquete(paquete, &size_a_enviar);

    send(socket, a_enviar, size_a_enviar, 0);

    paquete_detroy(paquete);

    free(a_enviar);
}

t_persona* recibir_persona(int socket){
    t_buffer* buffer = malloc(sizeof(t_buffer));
    
    recv(socket, &buffer->size, sizeof(uint32_t), MSG_WAITALL);
    buffer->stream = malloc(buffer->size);
    recv(socket, buffer->stream, buffer->size, MSG_WAITALL);

    buffer->offset = 0;

    t_persona* tmp = deserializar_persona(buffer);

    buffer_destroy(buffer);

    return tmp;
}
