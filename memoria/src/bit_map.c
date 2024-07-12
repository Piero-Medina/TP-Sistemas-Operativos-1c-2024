#include "bit_map.h"

t_bitmap* bitmap_crear(int cantidad_de_frames){
    t_bitmap* bitmap = malloc(sizeof(t_bitmap));

    bitmap->tamanio_en_bytes = cantidad_de_frames / 8;
    bitmap->direccion = malloc(bitmap->tamanio_en_bytes);

    // Inicializar la memoria asignada a cero
    memset(bitmap->direccion, 0, bitmap->tamanio_en_bytes);

    bitmap->bitarray = bitarray_create_with_mode(bitmap->direccion, bitmap->tamanio_en_bytes, LSB_FIRST);    
    
    return bitmap;
}

int bitmap_bloque_libre(t_bitmap* bitmap){   
    bool bloque_ocupado;

    for(int i = 0; i < (bitmap->tamanio_en_bytes * 8); i++){   
        bloque_ocupado = bitarray_test_bit(bitmap->bitarray, i);       
        if(!bloque_ocupado){
            return i;
        }
    }
    return -1;
}

void bitmap_marcar_bloque_como_libre(t_bitmap* bitmap, int numero_bloque){
    bitarray_clean_bit(bitmap->bitarray, numero_bloque);
}

void bitmap_marcar_bloque_como_ocupado(t_bitmap* bitmap, int numero_bloque){
    bitarray_set_bit(bitmap->bitarray, numero_bloque);
}

int bitmap_cantidad_de_bloques_libres(t_bitmap* bitmap){
    int contador = 0;
    bool bloque_ocupado;

    for(int i = 0; i < (bitmap->tamanio_en_bytes * 8); i++){   
        bloque_ocupado = bitarray_test_bit(bitmap->bitarray, i);       
        if (!bloque_ocupado) {
            contador ++;
        }
    }
    return contador;
}

void bitmap_mostrar_detalles(t_bitmap* bitmap){   
    int cantidad_de_bloques = 0;
    int cantidad_de_bloques_libres = 0;
    int cantidad_de_bloques_ocupados = 0;
    bool bloque;

    printf("Detalles del Bitmap:\n");
    printf("--------------------\n");

    for (int i = 0; i < (bitmap->tamanio_en_bytes * 8); i++){
        bloque = bitarray_test_bit(bitmap->bitarray, i);

        if(bloque == false){
            printf("| Block(%d)-> 0 |\n", i);
            cantidad_de_bloques_libres ++;
        }
        else{
            printf("| Block(%d)-> 1 |\n", i);
            cantidad_de_bloques_ocupados ++;
        }
        cantidad_de_bloques ++;
    }

    printf("--------------------\n");
    printf("Cantidad de bloques: %d \n", cantidad_de_bloques);
    printf("Cantidad de bloques libres: %d \n", cantidad_de_bloques_libres);
    printf("Cantidad de bloques ocupados: %d \n", cantidad_de_bloques_ocupados);

}

void bitmap_liberar(t_bitmap* bitmap){
    bitarray_destroy(bitmap->bitarray);
    free(bitmap->direccion);
    free(bitmap);
}