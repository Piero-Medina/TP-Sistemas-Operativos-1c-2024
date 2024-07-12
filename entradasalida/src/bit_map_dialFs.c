#include "bit_map_dialFs.h"

t_bitmap* bitmap_crear(char* path_bitmap, int cantidad_de_bloques){
    t_bitmap* bitmap = malloc(sizeof(t_bitmap));

    // abrir archivo en modo W y R, si no existe lo crea
    int file_descriptor = open(path_bitmap, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if(file_descriptor == -1){
        printf("Error al abrir el archivo Bitmap: %s \n", path_bitmap);
        return NULL;
    }

    bitmap->tamanio_en_bytes = cantidad_de_bloques / 8;

    // truncamos el archivo
    if(ftruncate(file_descriptor, bitmap->tamanio_en_bytes) == -1){
        printf("Error al truncar el archivo Bitmap: %s \n", path_bitmap);
        close(file_descriptor);
        return NULL;
    }

    // mapeamos en memoria el archivo
    bitmap->direccion = mmap(NULL, bitmap->tamanio_en_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor, 0);
    if(bitmap->direccion == MAP_FAILED){
        printf("Error al mapear el Bitmap: %s \n", path_bitmap);
        close(file_descriptor);
        return NULL;
    }

    bitmap->bitarray = bitarray_create_with_mode(bitmap->direccion, bitmap->tamanio_en_bytes, LSB_FIRST);

    close(file_descriptor);
    return bitmap;
}

//
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
    
    // Sincronizar los cambios en el archivo 
    if(msync(bitmap->direccion, bitmap->tamanio_en_bytes, MS_SYNC) == -1){
        printf("Error al sincronizar los cambios en el Bitmap \n");
        raise(SIGINT);
    }
}

void bitmap_marcar_bloque_como_ocupado(t_bitmap* bitmap, int numero_bloque){
    bitarray_set_bit(bitmap->bitarray, numero_bloque);

    // Sincronizar los cambios en el archivo
    if(msync(bitmap->direccion, bitmap->tamanio_en_bytes, MS_SYNC) == -1){
        printf("Error al sincronizar los cambios en el Bitmap \n");
        raise(SIGINT);
    }
}

//
int bitmap_cant_bloques_libres_contiguos_desde_posicion(t_bitmap* bitmap, int cant_esperada, int posicion){
    int contador = 0;
    int tamanio_en_bits = (bitmap->tamanio_en_bytes * 8);
    bool bloque_ocupado;

    for(int i = 0; i < cant_esperada; i++){
        // por si el ultimo bloque estaba libre, asi no lee mas del largo del bitmap
        if(posicion == (tamanio_en_bits + 1)){
            return contador;
        }

        bloque_ocupado = bitarray_test_bit(bitmap->bitarray, posicion);      
        if (!bloque_ocupado) {
            contador ++;
        }
        else{
            return contador;
        }

        posicion ++;
    }
    return contador;
}

int bitmap_posicion_bloque_contiguo_de_n_bloques(t_bitmap* bitmap, int cant_bloques){
    int bloque_inicial = 0;
    int bloques_libres = 0;
    int tamanio_en_bits = (bitmap->tamanio_en_bytes * 8);
    bool bloque_ocupado;

    while (bloque_inicial <= tamanio_en_bits){
        bloque_ocupado = bitarray_test_bit(bitmap->bitarray, bloque_inicial);
        if (!bloque_ocupado) {
            bloques_libres = bitmap_cant_bloques_libres_contiguos_desde_posicion(bitmap, cant_bloques, bloque_inicial);
            
            if(bloques_libres == cant_bloques){
                return bloque_inicial;
            }
            // si no es igual, solo puede ser menor o mayor a 0
            bloque_inicial += bloques_libres;
        }
        else{
            bloque_inicial ++;
        }
    }

    return -1;
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

void bitmap_marcar_bloques_ocupados_desde_posicion(t_bitmap* bitmap, int posicion, int cant_bloques){
    for(int i = 0; i < cant_bloques; i++){
        bitmap_marcar_bloque_como_ocupado(bitmap, posicion);         
        posicion ++;
    }
}

void bitmap_marcar_bloques_libres_desde_posicion(t_bitmap* bitmap, int posicion, int cant_bloques){
    for(int i = 0; i < cant_bloques; i++){
        bitmap_marcar_bloque_como_libre(bitmap, posicion);       
        posicion ++;
    }
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

        if(bloque == 0){
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
    munmap(bitmap->direccion, bitmap->tamanio_en_bytes); 
    bitarray_destroy(bitmap->bitarray);
    free(bitmap);
}