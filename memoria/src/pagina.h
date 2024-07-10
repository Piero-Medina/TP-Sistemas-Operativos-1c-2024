#ifndef PAGINA_H
#define PAGINA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <commons/collections/list.h>
#include <commons/log.h>

#include "generales.h"

typedef struct {
	uint32_t numero_pagina; // redundante
	uint32_t numero_frame;
}t_pagina;

t_pagina* crear_pagina(uint32_t numero_pagina, uint32_t numero_frame);

void liberar_elemento_pagina(void* elemento);
void liberar_lista_de_paginas(t_list* lista);

#endif 