#ifndef CPU_CONFIG_H
#define CPU_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>

// Definición de la estructura para la configuración de la CPU
typedef struct {
    t_config* config;
    char* ip_memoria;
    char* puerto_memoria;
    char* ip_cpu;
    char* puerto_escucha_dispatch;
    char* puerto_escucha_interrupt;
    int cantidad_entradas_tlb;
    char* algoritmo_tlb;
} t_cpu_config;

// Función para inicializar la configuración de la CPU desde un archivo
t_cpu_config* init_cpu_config(char* path);

// Función para liberar la memoria asignada a la configuración de la CPU
void liberar_cpu_config(t_cpu_config* cpu_config);

#endif 
