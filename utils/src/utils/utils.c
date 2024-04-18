#include <utils/utils.h>

char* string_aplanar_PID(t_list* lista) {
    char* tmp = malloc(sizeof(char) * MAX_SIZE);
    
    if (lista->elements_count == 0) {
        strcpy(tmp, "[vacio]");
        return tmp;
    }

	t_link_element **indirect = &lista->head;

    char convertido[10];
    t_PCB* pcb = NULL;

    tmp[0] = '\0';
    strcat(tmp, "[");

	while ((*indirect) != NULL) {
		pcb = (t_PCB*) (*indirect)->data;
        
        sprintf(convertido, "%d", pcb->pid);
        strcat(tmp, convertido);
        strcat(tmp, ",");

		indirect = &(*indirect)->next;
	}
    
    size_t longitud = strlen(tmp);
    tmp[longitud - 1] = '\0';

    strcat(tmp, "]");

    return tmp;
}

