#ifndef TLB_H
#define TLB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <sys/time.h>

#include <commons/collections/list.h>
#include <commons/log.h>

typedef enum{
    FIFO,
    LRU
}algoritmo;

typedef struct {
    int32_t pid;
    int32_t pagina;
    int32_t marco;
    struct timeval instante_carga;
    struct timeval instante_ultima_referencia;
}t_entrada_tlb;

t_list* crear_tlb(int cantidad_entradas);

void imprimir_entrada_tlb(t_entrada_tlb* entrada);
void imprimir_lista_entrada_tlb_completa(t_list* lista);
void imprimir_lista_entrada_tlb_sin_tiempo(t_list* lista);

void destruir_tlb(t_list* tlb);

int32_t buscar_entrada_tlb(t_list* tlb, algoritmo algoritmo_elegido, int32_t pid, int32_t numero_pagina, t_log* logger);

void agregar_entrada_tlb(t_list* tlb, algoritmo algoritmo_elegido, int32_t pid, int32_t numero_pagina, int32_t numero_marco);

void reemplazar_entrada_instante_de_carga_mas_viejo(t_list* tlb, int32_t pid, int32_t numero_pagina, int32_t numero_marco, struct timeval actual);
void reemplazar_entrada_instante_ultima_referencia_mas_viejo(t_list* tlb, int32_t pid, int32_t numero_pagina, int32_t numero_marco, struct timeval actual);

void limpiar_entrada_tlb(t_list* tlb, int indice);

#endif 