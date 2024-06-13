#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <commons/log.h>

#include "generales.h"
#include "init.h"

/*----------------------------------- Definicion de structs ----------------------------------- */

/*----------------------------------- Definicion de funciones -----------------------------------*/
/*Crear memoria*/
t_memoria_paginada* inicializar_memoria_paginada(int tamano, int tamano_pagina, t_log* logger);
/*liberar memoria*/
void libera_espacio_memoria(t_memoria_paginada* memoria,t_log* logger);


/*--------------------------------- Manipulacion de Memoria ----------------------------------*/

//void* leer_memoria(t_memoria_paginada* memoria,t_log* logger);
bool leer_memoria(t_memoria_paginada* memoria,int marco, int offset,t_log* logger);

//void* escribir_memoria(t_memoria_paginada* memoria,t_pagina* datos,t_log* logger);
bool escribir_memoria(t_memoria_paginada* memoria,int marco, int offset,t_log* logger);

/*--------------------------------- Gestion de procesos ----------------------------------*/
//void* cargar_proceso_en_memoria();
t_pagina* esta_libre(t_memoria_paginada* memoria,t_log* logger);
bool asignar_paginas_a_proceso(t_proceso* proceso,t_memoria_paginada* memoria,t_log* logger);
bool liberar_paginas_de_proceso(t_proceso* proceso,t_memoria_paginada* memoria,int marco, t_log* logger);