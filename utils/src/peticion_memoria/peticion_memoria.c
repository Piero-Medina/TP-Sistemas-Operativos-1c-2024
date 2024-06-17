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