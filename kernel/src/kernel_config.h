#ifndef KERNEL_CONFIG_H_
#define KERNEL_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

typedef struct {
	t_config* config;
	char* ip_kernel;
	char* puerto_kernel;
	char* ip_memoria;
	char* puerto_memoria;
	char* ip_cpu;
	char* puerto_cpu_dispatch;
    char* puerto_cpu_interrupt;
	char* algoritmo_planificacion;
	int quantum;
	char** recursos;
	char** instancias_recursos;
    int grado_max_multiprogramacion;
}t_kernel_config;

t_kernel_config* init_kernel_config(char* path);

void liberar_kernel_config(t_kernel_config* kernel_config);

#endif 