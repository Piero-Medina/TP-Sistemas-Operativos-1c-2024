#include "pagina.h"


t_pagina* crear_pagina(uint32_t numero_pagina, uint32_t numero_frame){
    t_pagina* pagina = (t_pagina*)malloc(sizeof(t_pagina));
    if (pagina == NULL) {
        printf("Error al asignar memoria para t_pagina\n");
        return NULL;
    }
    pagina->numero_pagina = numero_pagina;
    pagina->numero_frame = numero_frame;
    return pagina;
}

void liberar_elemento_pagina(void* elemento){
    t_pagina* pagina = (t_pagina*) elemento;

    if (pagina == NULL) {
        return;
    }

    free(pagina);
}

void liberar_lista_de_paginas(t_list* lista){
    list_destroy_and_destroy_elements(lista, liberar_elemento_pagina);
}