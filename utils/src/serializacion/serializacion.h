#ifndef STATIC_SERIALIZACION_H
#define STATIC_SERIALIZACION_H

 #include <stdlib.h>
 #include <stdio.h>
 #include <stdint.h> // para los diferentes int
 #include <string.h>
 #include <sys/socket.h>
 #include <commons/log.h>

 /*
   1 -> primero creamos un buffer para meter todos los datos a serializar
   2 -> creamos un paquete con el buffer adentro, asi definimos el protocolo
   3 -> serializamos el paquete por separado (op_code + size_stream + stream) y lo enviamos
   4 -> al recibir optenemos el op_code. luego el size_stream para pedir memoria, y luego
        el stream en si. (en el receptor creamos un t_buffer con offset en 0, asi podemos
        desearializar usando las funciones definidas) (en el receptor no tiene sentido crear
        un t_paquete por eso no lo mandamos a serializar)
 */
 
 typedef struct {
    uint32_t size; // Tamaño del payload
    uint32_t offset; // Desplazamiento dentro del payload
    void* stream; // Payload
 }t_buffer;

 typedef struct {
    uint8_t codigo_operacion; 
    t_buffer* buffer;
 }t_paquete;

 // - crea un paquete con un buffer inicializado con un tamanio especifico
 t_paquete* paquete_create_with_buffer_size(uint32_t size_buffer, uint8_t codigo_operacion);

 // - crea un paquete recibiendo un buffer externo
 t_paquete* paquete_create_add_buffer(t_buffer* buffer, uint8_t codigo_operacion);
 
 // - crea un paquete con un buffer NULL (vacio)
 t_paquete* paquete_create_with_buffer_null(uint8_t codigo_operacion);

 // - Libera la memoria asociada a un paquete
 void paquete_detroy(t_paquete* paquete); 
 
 // - Crea un buffer de tamanio (size), offset (0) y reservando memoria para stream (size)
 t_buffer* buffer_create(uint32_t size);

 // - Libera la memoria asociada al buffer
 void buffer_destroy(t_buffer* buffer);

 // - Agrega un stream al buffer en la posición actual y avanza el offset
 void buffer_add(t_buffer *buffer, void* data, uint32_t size);

 // - Guarda size bytes del principio del buffer en la dirección data y avanza el offset
 void buffer_read(t_buffer* buffer, void* data, uint32_t size);


// --------------------------  agregar tipos de dato -----------------------

 // - Agrega un int al buffer
 void buffer_add_int(t_buffer *buffer, int data);

 // - Agrega un uint32_t al buffer
 void buffer_add_uint32(t_buffer *buffer, uint32_t data);

 // - Agrega un uint8_t al buffer
 void buffer_add_uint8(t_buffer *buffer, uint8_t data);

 // - Agrega string al buffer con un uint32_t adelante indicando su longitud
 void buffer_add_string(t_buffer *buffer, uint32_t length, char *string);

// --------------------------  leer tipos de dato -----------------------

 // - Lee un int del buffer y avanza el offset
 int buffer_read_int(t_buffer *buffer);

 // - Lee un uint32_t del buffer y avanza el offset
 uint32_t buffer_read_uint32(t_buffer *buffer);

 // - Lee un uint8_t del buffer y avanza el offset 
 uint8_t buffer_read_uint8(t_buffer *buffer);

 // - Lee un string y su longitud del buffer y avanza el offset
 char* buffer_read_string(t_buffer *buffer);

//-------------------------------------- EXTRAS --------------------------------

 // - serializamos un paquete, retorno un stream con el paquete serializado
 // - (liberar el stream (void*) y el paquete)
 void* serializar_paquete(t_paquete* paquete, uint32_t* size_a_enviar);

 // - Recibe un buffer por red, se deserializa e inicializa el offset en 0  
 // - (liberar el buffer cuando se termine de sacar todo)
 t_buffer* recibir_buffer(int conexion);

//-------------------------------------- HANDSHAKE ----------------------------------------------
 
 // envia handshake al servidor e informa si fue exitoso
 void enviar_handshake(int conexion, uint8_t codigo_operacion, char* modulo_origen, char* modulo_destino, t_log* logger);
 
 // retorna de que modulo_cliente se realiza el handshake
 char* recibir_handshake(int fd_cliente);

 // nos fijamos si el handshake fue exitoso
 void estado_handshake(int conexion, char* modulo, t_log* logger);

 // respondemos que el handShake fue exitoso
 void responder_handshake(int fd_cliente);

//-------------------------------------- EJEMPLO -------------------------------------------------
 
 // 
 typedef struct {
    uint32_t dni;
    uint8_t edad;
    uint32_t pasaporte;
    char* nombre;
 } t_persona;

 // imprime cada campo de un t_persona 
 void ver_persona(t_persona* persona);
 
 // crea y asigna memoria para un t_persona
 t_persona* crear_persona(uint32_t dni, uint8_t edad, uint32_t pasaporte, const char* nombre);

 // libera memoeria asociada a un t_persona 
 void liberar_persona(t_persona* persona);

 // serializa un t_persona y retorna un t_buffer 
 t_buffer* serializar_persona(t_persona* persona);

 // deserializa un t_persona y la retorna
 t_persona* deserializar_persona(t_buffer* buffer);

 // envia por red un t_persona 
 void enviar_persona(int socket, t_persona* persona, uint8_t codigo_operacion);

 // recibe por red un t_persona
 t_persona* recibir_persona(int socket);

#endif