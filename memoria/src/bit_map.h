#ifndef BIT_MAP_H
#define BIT_MAP_H

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h> // open() ...
#include <sys/mman.h> // mmap()
#include <unistd.h>   // msync() 
#include <stdbool.h>
#include <signal.h>

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

// -retorna la cantidad de bloques libres desde una tal posicion (se tiene en cuenta la posicion actual)
// -se asume que el bit a pasar esta libre, desde ahi comienza a contar  por lo tanto nunca retornara 0;
// -si hay la cant_esperada de bloques libres -> retornara la cant de bloques esperado
// -si no hay la cant_esperada de bloques libres -> retorna la cant de bloques libres que encontro
// -lo que retorne nunca sera mayor a la cant_esperada ni menor a 1.
// -si el primer bit a evaluar (posicion) no esta libre -> retornara 0.
int bitmap_cant_bloques_libres_contiguos_desde_posicion(t_bitmap* bitmap, int cant_esperada, int posicion);

// -retorna el numero de bloque inicial del cual hay n cant_bloques contiguos luego del inicial (contando el inicial)
// -si no encuentra una cant_bloques contiguos retorna -1
// -puede retornar el bloque 0;
int bitmap_posicion_bloque_contiguo_de_n_bloques(t_bitmap* bitmap, int cant_bloques);

// -retorna la cant de bloques libres (literal) total de todo el bitmap
int bitmap_cantidad_de_bloques_libres(t_bitmap* bitmap);

// -marca y cuenta a partir de la posicion que se le pase.
// -posicion = 1 y cant = 2 -> (marca bloque 1,2 como ocupado)
void bitmap_marcar_bloques_ocupados_desde_posicion(t_bitmap* bitmap, int posicion, int cant_bloques);
void bitmap_marcar_bloques_libres_desde_posicion(t_bitmap* bitmap, int posicion, int cant_bloques);

//
void bitmap_mostrar_detalles(t_bitmap* bitmap);

// libera un bitmap y lo desmapea de memoria
void bitmap_liberar(t_bitmap* bitmap);

#endif