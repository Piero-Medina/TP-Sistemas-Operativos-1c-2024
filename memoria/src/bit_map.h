#ifndef BIT_MAP_H
#define BIT_MAP_H

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h> // open() ...
#include <sys/mman.h> // mmap()
#include <unistd.h>   // msync() 
#include <stdbool.h>
#include <signal.h>
#include <string.h>

#include <commons/bitarray.h>

typedef struct{
    char* direccion;
    int tamanio_en_bytes;         
    t_bitarray *bitarray; 
}t_bitmap;

// el bitmap tiene el tamanio en base 0;
// false = 0 -> libre
// true = 1 -> ocupado

// - abre un bitmap y lo mapea a memoria (si no existe lo crea), devuelve NULL ante no poder crear un bitmap
t_bitmap* bitmap_crear(int cantidad_de_bloques);

// - devuelve el primer numero de bloque libre encontrado del bitmap
int bitmap_bloque_libre(t_bitmap* bitmap);

void bitmap_marcar_bloque_como_libre(t_bitmap* bitmap, int numero_bloque);
void bitmap_marcar_bloque_como_ocupado(t_bitmap* bitmap, int numero_bloque);

// - retorna la cantidad de bloques libres (literal) total de todo el bitmap
int bitmap_cantidad_de_bloques_libres(t_bitmap* bitmap);

// - estadisticas del bitmap
void bitmap_mostrar_detalles(t_bitmap* bitmap);

// libera un bitmap y lo desmapea de memoria
void bitmap_liberar(t_bitmap* bitmap);

#endif