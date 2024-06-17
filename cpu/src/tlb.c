#include "tlb.h"

t_list* crear_tlb(int cantidad_entradas){
    t_list* tlb_tmp = list_create();
    
    for (int i = 0; i < cantidad_entradas; i++){
		t_entrada_tlb *entrada = malloc(sizeof(t_entrada_tlb));

        entrada->pid = -1;
		entrada->pagina = -1;
		entrada->marco = -1;

		entrada->instante_carga.tv_sec = 0;
		entrada->instante_carga.tv_usec = 0;

		entrada->instante_ultima_referencia.tv_sec = 0;
		entrada->instante_ultima_referencia.tv_usec = 0;

		list_add(tlb_tmp, (void*) entrada);
	}
    return tlb_tmp;
}

void imprimir_entrada_tlb(t_entrada_tlb* entrada){
    if (entrada == NULL) {
        fprintf(stderr, "Puntero nulo.\n");
        return;
    }

    printf("Pid: %d\n", entrada->pid);
    printf("Pagina: %d\n", entrada->pagina);
    printf("Marco: %d\n", entrada->marco);
    printf("Instante de Carga: (%ld) segundos, (%06ld) microsegundos\n", entrada->instante_carga.tv_sec, entrada->instante_carga.tv_usec);
    printf("Instante de Ultima Referencia: (%ld) segundos, (%06ld) microsegundos\n", entrada->instante_ultima_referencia.tv_sec, entrada->instante_ultima_referencia.tv_usec);
}

void imprimir_lista_entrada_tlb_completa(t_list* lista){
    if (lista == NULL) {
        fprintf(stderr, "Puntero nulo a la lista.\n");
        return;
    }

    int tamanio = list_size(lista);
    for (int i = 0; i < tamanio; i++) {
        t_entrada_tlb* entrada = (t_entrada_tlb*) list_get(lista, i);
        printf("Entrada (%d) [PID %d | Pagina %d | Marco %d | Instante de Carga %ld segundos %06ld microsegundos | Instante de Ultima Referencia %ld segundos %06ld microsegundos]\n",
           i, entrada->pid, entrada->pagina, entrada->marco,
           entrada->instante_carga.tv_sec, entrada->instante_carga.tv_usec,
           entrada->instante_ultima_referencia.tv_sec, entrada->instante_ultima_referencia.tv_usec);
    }
}

void imprimir_lista_entrada_tlb_sin_tiempo(t_list* lista) {
    if (lista == NULL) {
        fprintf(stderr, "Puntero nulo a la lista.\n");
        return;
    }

    int tamanio = list_size(lista);
    for (int i = 0; i < tamanio; i++) {
        t_entrada_tlb* entrada = (t_entrada_tlb*) list_get(lista, i);
        printf("Entrada (%d) [PID %d | Pagina %d | Marco %d]\n", i, entrada->pid, entrada->pagina, entrada->marco);
    }
}


void destruir_tlb(t_list* tlb){
    list_destroy_and_destroy_elements(tlb, free);
}

int32_t buscar_entrada_tlb(t_list* tlb, algoritmo algoritmo_elegido, int32_t pid, int32_t numero_pagina, t_log* logger){
    int size_lista = list_size(tlb);

	for (int i = 0; i < size_lista; i++){
		t_entrada_tlb* entrada = (t_entrada_tlb*) list_get(tlb, i);

		if ((entrada->pid == pid) && (entrada->pagina == numero_pagina)){
			log_warning(logger, "PID: <%d> - TLB HIT - Pagina: <%d>", (int)pid, (int)entrada->pagina);

			if(algoritmo_elegido == LRU){
				gettimeofday(&entrada->instante_ultima_referencia, NULL);
			}

			return entrada->marco;
		}
	}

    log_warning(logger, "PID: <%d> - TLB MISS - Pagina: <%d>", (int)pid, (int)numero_pagina);
	return -1;
}

void agregar_entrada_tlb(t_list* tlb, algoritmo algoritmo_elegido, int32_t pid, int32_t numero_pagina, int32_t numero_marco){
	struct timeval actual;
    gettimeofday(&actual, NULL);

	for (int i = 0; i < list_size(tlb); i++){
		t_entrada_tlb* entrada = list_get(tlb, i);
		
		if (entrada->pid == -1){
			entrada->pid = pid;
			entrada->pagina = numero_pagina;
			entrada->marco = numero_marco;

			if(algoritmo_elegido == FIFO){
				entrada->instante_carga = actual;	
			}

			if(algoritmo_elegido == LRU){
				entrada->instante_ultima_referencia = actual;
			}

			return;
		}
	}

	// - TLB llena

	if(algoritmo_elegido == FIFO){
		reemplazar_entrada_instante_de_carga_mas_viejo(tlb, pid, numero_pagina, numero_marco, actual);
	}

	if(algoritmo_elegido == LRU){
		reemplazar_entrada_instante_ultima_referencia_mas_viejo(tlb, pid, numero_pagina, numero_marco, actual);
	}

}

// Función para encontrar y reemplazar la entrada más vieja
void reemplazar_entrada_instante_de_carga_mas_viejo(t_list* tlb, int32_t pid, int32_t numero_pagina, int32_t numero_marco, struct timeval actual){
    int indice_mas_viejo = 0;
	int size_lista = list_size(tlb);

	t_entrada_tlb* entrada1 = (t_entrada_tlb*) list_get(tlb, indice_mas_viejo);
	t_entrada_tlb* entrada2 = NULL;

    for (int i = 1; i < size_lista; ++i){
		entrada2 = (t_entrada_tlb*) list_get(tlb, i);
        if (timercmp(&entrada1->instante_carga, &entrada2->instante_carga, >)) {
            indice_mas_viejo = i;
			entrada1 = (t_entrada_tlb*) list_get(tlb, indice_mas_viejo);
        }
    }

	limpiar_entrada_tlb(tlb, indice_mas_viejo);

	entrada1 = (t_entrada_tlb*) list_get(tlb, indice_mas_viejo);

	entrada1->pid = pid;
	entrada1->pagina = numero_pagina;
	entrada1->marco = numero_marco;

	entrada1->instante_carga = actual;
}

void reemplazar_entrada_instante_ultima_referencia_mas_viejo(t_list* tlb, int32_t pid, int32_t numero_pagina, int32_t numero_marco, struct timeval actual){
    int indice_mas_viejo = 0;
	int size_lista = list_size(tlb);

	t_entrada_tlb* entrada1 = (t_entrada_tlb*) list_get(tlb, indice_mas_viejo);
	t_entrada_tlb* entrada2 = NULL;

    for (int i = 1; i < size_lista; ++i){
		entrada2 = (t_entrada_tlb*) list_get(tlb, i);
        if (timercmp(&entrada1->instante_ultima_referencia, &entrada2->instante_ultima_referencia, >)) {
            indice_mas_viejo = i;
			entrada1 = (t_entrada_tlb*) list_get(tlb, indice_mas_viejo);
        }
    }

	limpiar_entrada_tlb(tlb, indice_mas_viejo);

	entrada1 = (t_entrada_tlb*) list_get(tlb, indice_mas_viejo);

	entrada1->pid = pid;
	entrada1->pagina = numero_pagina;
	entrada1->marco = numero_marco;

	entrada1->instante_ultima_referencia = actual;
}

void limpiar_entrada_tlb(t_list* tlb, int indice){
	t_entrada_tlb* entrada = (t_entrada_tlb*) list_get(tlb, indice);

	entrada->pid = -1;
	entrada->pagina = -1;
	entrada->marco = -1;

	entrada->instante_carga.tv_sec = 0;
	entrada->instante_carga.tv_usec = 0;

	entrada->instante_ultima_referencia.tv_sec = 0;
	entrada->instante_ultima_referencia.tv_usec = 0;
}

