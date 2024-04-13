#include "kernel_config.h"

t_kernel_config* init_kernel_config(char* path){

	t_kernel_config* kernel_config = malloc(sizeof(t_kernel_config));

	kernel_config->config = config_create(path);

	if(kernel_config->config == NULL){
		printf("ERROR al abrir el .config\n");
		exit(EXIT_FAILURE);
	}

	kernel_config->ip_kernel = config_get_string_value(kernel_config->config, "IP_KERNEL");
	kernel_config->puerto_kernel = config_get_string_value(kernel_config->config, "PUERTO_KERNEL"); 
	kernel_config->ip_memoria = config_get_string_value(kernel_config->config, "IP_MEMORIA");
	kernel_config->puerto_memoria = config_get_string_value(kernel_config->config, "PUERTO_MEMORIA");
	kernel_config->ip_cpu = config_get_string_value(kernel_config->config, "IP_CPU");
	kernel_config->puerto_cpu_dispatch = config_get_string_value(kernel_config->config, "PUERTO_CPU_DISPATCH");
    kernel_config->puerto_cpu_interrupt = config_get_string_value(kernel_config->config, "PUERTO_CPU_INTERRUPT");
	kernel_config->algoritmo_planificacion = config_get_string_value(kernel_config->config, "ALGORITMO_PLANIFICACION");
	kernel_config->quantum = config_get_int_value(kernel_config->config, "QUANTUM");
	kernel_config->recursos = config_get_array_value(kernel_config->config, "RECURSOS");
	kernel_config->instancias_recursos = config_get_array_value(kernel_config->config, "INSTANCIAS_RECURSOS");
    kernel_config->grado_max_multiprogramacion = config_get_int_value(kernel_config->config, "GRADO_MULTIPROGRAMACION");

	return kernel_config;
}

void liberar_kernel_config(t_kernel_config* kernel_config) {

	config_destroy(kernel_config->config);

	// no es necesario - son cadenas literales
    //free(kernel_config->ip_kernel);
    //free(kernel_config->puerto_kernel);
    //free(kernel_config->ip_memoria);
    //free(kernel_config->puerto_memoria);
    //free(kernel_config->ip_cpu);
    //free(kernel_config->puerto_cpu_dispatch);
    //free(kernel_config->puerto_cpu_interrupt);
    //free(kernel_config->algoritmo_planificacion);

    string_array_destroy(kernel_config->recursos);
    string_array_destroy(kernel_config->instancias_recursos);

    free(kernel_config);
}
