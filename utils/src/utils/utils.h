#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <commons/collections/node.h>

#include <pcb/pcb.h>

#define MAX_SIZE 80

char* string_aplanar_PID(t_list* lista);

// funciones usadas para simular IO por el momento
void hilo_dormir_milisegundos(int milisegundos);
void mimir_milisegundos(void* args);
void sleep_ms(int milisegundos);


#endif