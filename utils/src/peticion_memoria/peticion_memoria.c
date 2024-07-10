#include <peticion_memoria/peticion_memoria.h>

t_peticion_memoria* crear_peticion_memoria(uint32_t base, uint32_t direccion_fisica, uint32_t bytes){
    t_peticion_memoria* peticion = malloc(sizeof(t_peticion_memoria));
    
    if (peticion == NULL) {
        fprintf(stderr, "Error al asignar memoria.\n");
        exit(EXIT_FAILURE);
    }
    
    peticion->base = base;
    peticion->direccion_fisica = direccion_fisica;
    peticion->bytes = bytes;
    
    return peticion;
}

void imprimir_peticion_memoria(t_peticion_memoria* peticion){
    if (peticion == NULL) {
        fprintf(stderr, "Puntero nulo.\n");
        return;
    }
    printf("Base: %u\n", peticion->base);
    printf("Direccion Fisica: %u\n", peticion->direccion_fisica);
    printf("Bytes: %u\n", peticion->bytes);
}

void imprimir_lista_peticion_memoria(t_list* lista){
    for (int i = 0; i < list_size(lista); i++) {
        t_peticion_memoria* peticion = (t_peticion_memoria* )list_get(lista, i);
        printf("Peticion (%d) [Base %u| Direccion Fisica %u | Bytes %u]\n", i, peticion->base, peticion->direccion_fisica, peticion->bytes);
    }
}

void liberar_elemento_peticion_memoria(void* elemento){
    t_peticion_memoria* peticion = (t_peticion_memoria*) elemento;

    if (peticion != NULL) {
        free(peticion);
    }
}

void liberar_lista_de_peticiones_memoria(t_list* lista){
    list_destroy_and_destroy_elements(lista, liberar_elemento_peticion_memoria);
}

void buffer_add_list_t_peticion_memoria(t_buffer* buffer, t_list* lista, uint32_t size_lista){
    t_peticion_memoria* tmp = NULL;

    // agrega al buffer el largo de la lista
    buffer_add_uint32(buffer, size_lista);

    for (int i = 0; i < size_lista; i++){
        tmp = list_get(lista, i);
        buffer_add_uint32(buffer, tmp->base);
        buffer_add_uint32(buffer, tmp->direccion_fisica);
        buffer_add_uint32(buffer, tmp->bytes);     
    }
}

t_list* buffer_read_list_t_peticion_memoria(t_buffer* buffer){
    t_list* tmp = list_create();
    t_peticion_memoria* peticion = NULL;

    // lee del buffer el largo de la lista
    uint32_t size_lista = buffer_read_uint32(buffer);

    // lee cada string del buffer y lo agrega a la lista
    for(int i = 0; i < size_lista; i++){
        peticion = malloc(sizeof(t_peticion_memoria));
        
        peticion->base = buffer_read_uint32(buffer);
        peticion->direccion_fisica = buffer_read_uint32(buffer);
        peticion->bytes = buffer_read_uint32(buffer);

        list_add(tmp, (void*) peticion);
        
        peticion = NULL;
    }

    return tmp;
}

t_buffer* serializar_lista_de_t_peticion_memoria(t_list* lista){
    uint32_t largo_lista = (uint32_t) list_size(lista);
    int size_lista_serializable = largo_lista * (3 * sizeof(t_peticion_memoria));

    uint32_t size = sizeof(uint32_t) +                // largo de la lista 
                    size_lista_serializable;          // largo total de elementos de la lista

    t_buffer* buffer = buffer_create(size);

    buffer_add_list_t_peticion_memoria(buffer, lista, largo_lista);
    
    return buffer;
}

t_list* deserializar_lista_de_t_peticion_memoria(t_buffer* buffer){
    return buffer_read_list_t_peticion_memoria(buffer);
}

bool gestionar_escritura_multipagina(int conexion_memoria, t_list* peticiones, uint32_t pid, void* data_a_escribir, uint32_t bytes, t_log* logger){
    void* buffer_test = malloc(bytes);
    int bytes_test = 0; 

    int offset = 0;

    t_peticion_memoria* tmp = NULL;
    for(int i = 0; i < list_size(peticiones); i++){
        tmp = (t_peticion_memoria*) list_get(peticiones, i);
                        
        // copia a partir del tamanio antes enviado,
        void* data_leida = malloc(tmp->bytes);
        memcpy(data_leida, data_a_escribir + offset, tmp->bytes);
        offset += tmp->bytes;

        // Solicitudes a memoria
        enviar_generico_doble_entero(conexion_memoria, SOLICITUD_ESCRITURA_MEMORIA, tmp->direccion_fisica, pid);
        enviar_data(conexion_memoria, IGNORAR_OP_CODE, data_leida, tmp->bytes);
        ignorar_op_code(conexion_memoria); // espero una respuesta para poder continuar iterando

        // seria un pedazo si es multipagina
        char* string_escrito = convertir_a_cadena_nueva(data_leida, tmp->bytes);
        log_info(logger, "PID: <%u> - Acción: <ESCRIBIR> - Dirección Física: <%u> - Valor: <%s>", pid, tmp->direccion_fisica, string_escrito);
        free(string_escrito);

        // alamcenamos en el (buffer_Test)para luego verificar si lo enviado coincide
        memcpy(buffer_test + bytes_test, data_leida, tmp->bytes);
        bytes_test+= tmp->bytes;

        free(data_leida);
    }

    // verificamos lo enviado (buffer_test) (si la data es un entero, es normal que muestre datos ramdom si es un string)
    char* final_escrito = convertir_a_cadena_nueva(buffer_test, bytes);
    log_info(logger, "PID: <%u> - Valor Final Escrito (%s)", pid, final_escrito);
    free(final_escrito);

    // liberamos lo almacenado (buffer_test)
    free(buffer_test);

    // verificando que la cantidad de bytes escritos coincide con lo esperado
    return (offset == bytes);
}

bool gestionar_lectura_multipagina(int conexion_memoria, t_list* peticiones, uint32_t pid, void** data_leida, uint32_t bytes, t_log* logger){
    void* buffer_data = malloc(bytes);
    int offset = 0;

    t_peticion_memoria* tmp = NULL;
    for(int i = 0; i < list_size(peticiones); i++){
        tmp = (t_peticion_memoria*) list_get(peticiones, i);
                        
        // Solicitudes a memoria
        enviar_generico_triple_entero(conexion_memoria, SOLICITUD_LECTURA_MEMORIA, tmp->direccion_fisica, tmp->bytes, pid);
        ignorar_op_code(conexion_memoria); // esperando a que me llegue para poder seguir iterando
        void* data = recibir_data(conexion_memoria, NULL);

        // seria un pedazo si es multipagina
        char* string_leido = convertir_a_cadena_nueva(data, tmp->bytes);
        log_info(logger, "PID: <%u> - Acción: <LEER> - Dirección Física: <%u> - Valor: <%s>", pid, tmp->direccion_fisica, string_leido);
        free(string_leido);

        // va almacenando la data en el buffer
        memcpy(buffer_data + offset, data, tmp->bytes);
        offset += tmp->bytes;

        free(data);
    }

    if(buffer_data == NULL){
        log_warning(logger,"buffer_data es NULL");
    }

    // esto no funciona como se espera, luego ver de si retornar data leida o mandar un puntero doble.
    *data_leida = buffer_data;

    // verificamos lo leido (buffer_test) (si la data es un entero, es normal que muestre datos ramdom si es un string)
    char* final_leido = convertir_a_cadena_nueva(*data_leida, bytes);
    log_info(logger, "PID: <%u> - Valor Final Leido (%s)", pid, final_leido);
    free(final_leido);

    // verificando que la cantidad de bytes leidos coincide con lo esperado
    return (offset == bytes);
}

