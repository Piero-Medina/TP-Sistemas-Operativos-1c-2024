#include "peticiones_general.h"

void* get_valor_memoria(uint32_t direcion_fisica, uint32_t tamanio){
    void* valor = malloc(tamanio); 
    memcpy(valor, memoria_real + direcion_fisica, tamanio);
    return valor;
}

void set_valor_en_memoria(uint32_t direcion_fisica, void* data, uint32_t tamanio){
    memcpy(memoria_real + direcion_fisica, data, tamanio);
}

