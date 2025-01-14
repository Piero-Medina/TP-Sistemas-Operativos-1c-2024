#include <utils/utils.h>

FILE* leer_archivo(char *path){
	FILE * archivo = fopen(path, "r");
	if(archivo == NULL){
		printf("no se pudo leer archivo \n");
		exit(EXIT_FAILURE);
	}
	return archivo;
}

void cerrar_archivo(FILE* archivo){
    fclose(archivo);
}

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
        
        sprintf(convertido, "%u", pcb->pid);
        strcat(tmp, convertido);
        strcat(tmp, ",");

		indirect = &(*indirect)->next;
	}
    
    size_t longitud = strlen(tmp);
    tmp[longitud - 1] = '\0';

    strcat(tmp, "]");

    return tmp;
}

char* string_aplanar_PID_int(t_list* lista) {
    char* tmp = malloc(sizeof(char) * MAX_SIZE);
    
    if (lista->elements_count == 0) {
        strcpy(tmp, "[vacio]");
        return tmp;
    }

	t_link_element **indirect = &lista->head;

    char convertido[10];
    int* pid = NULL;

    tmp[0] = '\0';
    strcat(tmp, "[");

	while ((*indirect) != NULL) {
		pid = (int*) (*indirect)->data;
        
        sprintf(convertido, "%d", *pid);
        strcat(tmp, convertido);
        strcat(tmp, ",");

		indirect = &(*indirect)->next;
	}
    
    size_t longitud = strlen(tmp);
    tmp[longitud - 1] = '\0';

    strcat(tmp, "]");

    return tmp;
}


void hilo_dormir_milisegundos(int milisegundos){
    pthread_t hilo_durmicion;
    int* milisegundos_ptr = malloc(sizeof(int));
    *milisegundos_ptr = milisegundos;
    pthread_create(&hilo_durmicion, NULL, (void*)mimir_milisegundos, (void*) milisegundos_ptr);
    pthread_join(hilo_durmicion, NULL);
}

void mimir_milisegundos(void* args){
    int* milisegundos = (int*) args;
    sleep_ms(*milisegundos);
    free(milisegundos);
}

void sleep_ms(int milisegundos) {
    usleep(milisegundos * 1000); 
}

char* concatenar_ruta(char* base, char* nombre_archivo){
    // Calcular la longitud total de la ruta completa
    size_t longitud_base = strlen(base);
    size_t longitud_archivo = strlen(nombre_archivo);

    // +1 para la barra '/' y +1 para el carácter nulo '\0'
    size_t longitud_ruta_completa = longitud_base + longitud_archivo + 2;

    // Asignar memoria para la ruta completa (un char es un byte, y malloc toma bytes enteros)
    char *ruta_completa = malloc(longitud_ruta_completa * sizeof(char));

    // Construir la ruta completa
    strcpy(ruta_completa, base);
    strcat(ruta_completa, "/");
    strcat(ruta_completa, nombre_archivo);

    return ruta_completa;
}

char* remover_extension(const char* nombre_archivo){
    // Encuentra la última ocurrencia del punto en el nombre del archivo
    const char* punto = strrchr(nombre_archivo, '.');
    
    size_t longitud;
    if (punto) {
        // Si hay un punto, calcula la longitud hasta el punto
        longitud = punto - nombre_archivo;
    } else {
        // Si no hay punto, la longitud es la del string completo
        longitud = strlen(nombre_archivo);
    }

    // Asigna memoria para el nuevo string
    char* nuevo_nombre = malloc(longitud + 1);

    // Copia el nombre del archivo sin la extensión
    strncpy(nuevo_nombre, nombre_archivo, longitud);
    
    // Asegúrate de que el nuevo string esté correctamente terminado en NULL
    nuevo_nombre[longitud] = '\0';
    
    return nuevo_nombre;
}

char* concatenar_nombre_y_extension(char* nombre, char* extension){
    // Calcular la longitud del nombre, la extensión y el carácter nulo
    size_t len_nombre = strlen(nombre);
    size_t len_extension = strlen(extension);
    size_t len_total = len_nombre + len_extension + 1;

    // Asignar memoria para el nuevo string
    char* resultado = malloc(len_total * sizeof(char));
    if (resultado == NULL) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }

    // Copiar el nombre y la extensión en el nuevo string
    strcpy(resultado, nombre);
    strcat(resultado, extension);

    return resultado;
}

int verificar_directorio(char* ruta){
    struct stat st;

    // Verificar si el directorio ya existe
    if (stat(ruta, &st) == 0) {
        //printf("El directorio ya existe.\n");
        return 0; // Retorna 0 indicando que el directorio ya existe
    } else {
        // Intentar crear el directorio
        if (mkdir(ruta, 0777) == 0) {
            //printf("Directorio creado exitosamente.\n");
            return 1; // Retorna 1 indicando que el directorio fue creado
        } else {
            //printf("Error al crear el directorio.\n");
            return -1; // Retorna -1 indicando que ocurrió un error al crear el directorio
        }
    }
}

int cantidad_de_bloques_ocupados(int bytes, int tamanio_bloque){
    double resultado = (double)bytes / (double)tamanio_bloque;
    
    if (resultado == (int)resultado) {
        return (int)resultado; // Si es un entero exacto, retorna el valor como entero
    } else {
        return (int)ceil(resultado); // Si no es un entero exacto, redondea hacia arriba
    }
}

int bytes_sobrantes_de_ultimo_bloque(int bytes, int tamanio_bloque){
    int resto, falta;

    resto = bytes % tamanio_bloque;
    falta = tamanio_bloque - resto;

    // agregado, si no hay fragmetnacion interna en el ultimo bloque de 0
    if(falta == tamanio_bloque){
        falta = 0;
        return falta;
    }

    return falta;
}

char* entero_a_string(int entero){
    // Suponemos que un entero no tendrá más de 20 dígitos
    char* cadena = malloc(sizeof(char) * 21); // Se reserva espacio para el máximo de 20 dígitos y el caracter nulo '\0'

    if (cadena == NULL) {
        printf("Error: No se pudo asignar memoria\n");
        exit(1); // O manejar el error de otra manera, según sea necesario
    }

    // Convertir el entero a una cadena
    sprintf(cadena, "%d", entero);
    return cadena;
}

int calcular_bloques_necesarios(int bytes, int tamanio_bloque){
    return (bytes + tamanio_bloque - 1) / tamanio_bloque;
}

char* eliminar_caracteres_finales(const char* buffer, char caracter){
    // Obtiene la longitud del buffer
    size_t longitud = strlen(buffer);

    // Encuentra la posición del último caracter diferente al 'caracter' dado
    int indice_final = longitud - 1;
    while (indice_final >= 0 && buffer[indice_final] == caracter) {
        indice_final--;
    }

    // Si el buffer está vacío o solo contiene caracteres 'caracter'
    if (indice_final < 0) {
        char* nuevo_buffer = malloc(1); // Solo un carácter para el carácter nulo
        if (nuevo_buffer == NULL) {
            perror("Error al asignar memoria");
            exit(EXIT_FAILURE);
        }
        nuevo_buffer[0] = '\0'; // Cadena vacía
        return nuevo_buffer;
    }

    // Copia el contenido del buffer al nuevo buffer hasta el último caracter diferente a 'caracter'
    char* nuevo_buffer = malloc(indice_final + 2); // +1 para el último caracter, +1 para el carácter nulo
    if (nuevo_buffer == NULL) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }
    strncpy(nuevo_buffer, buffer, indice_final + 1);
    nuevo_buffer[indice_final + 1] = '\0'; // Agrega el carácter nulo al final
    return nuevo_buffer;
}

char* eliminar_caracteres(const char* buffer, char caracter){
    // Obtiene la longitud del buffer
    size_t longitud = strlen(buffer);

    // Cuenta la cantidad de caracteres a mantener
    size_t cantidad_a_mantener = 0;
    for (size_t i = 0; i < longitud; i++) {
        if (buffer[i] != caracter) {
            cantidad_a_mantener++;
        }
    }

    // Si todos los caracteres son iguales a 'caracter', devuelve una cadena vacía
    if (cantidad_a_mantener == 0) {
        char* nuevo_buffer = malloc(1); // Solo un carácter para el carácter nulo
        if (nuevo_buffer == NULL) {
            perror("Error al asignar memoria");
            exit(EXIT_FAILURE);
        }
        nuevo_buffer[0] = '\0'; // Cadena vacía
        return nuevo_buffer;
    }

    // Crea un nuevo buffer y copia los caracteres diferentes a 'caracter'
    char* nuevo_buffer = malloc(cantidad_a_mantener + 1); // +1 para el carácter nulo
    if (nuevo_buffer == NULL) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }

    size_t j = 0;
    for (size_t i = 0; i < longitud; i++) {
        if (buffer[i] != caracter) {
            nuevo_buffer[j++] = buffer[i];
        }
    }
    nuevo_buffer[cantidad_a_mantener] = '\0'; // Agrega el carácter nulo al final
    return nuevo_buffer;
}


char* remover_primer_char_si_machea(const char* str, char char_to_remove){
    size_t len = strlen(str);

    // Verificar si el primer carácter es el que queremos eliminar
    if (str[0] == char_to_remove) {
        // Asignar memoria para la nueva cadena, sin el primer carácter
        char* new_str = (char*)malloc((len) * sizeof(char));
        if (new_str == NULL) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }

        // Copiar la cadena original a la nueva cadena, omitiendo el primer carácter
        strcpy(new_str, str + 1);

        return new_str;
    } else {
        // Asignar memoria para la nueva cadena con la misma longitud que la cadena original
        char* new_str = (char*)malloc((len + 1) * sizeof(char));
        if (new_str == NULL) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }

        // Copiar la cadena original a la nueva cadena
        strcpy(new_str, str);

        return new_str;
    }
}


uint32_t cadena_a_valor_entero(void* cadena, size_t bytes){
    uint32_t valor = 0;

    /*
    if (bytes != 1 && bytes != 4) {
        return 0;
    }
    */

    // si no almacenamos en un uint8_t, no podra respresentar el valor entero.
    // una vez representado, hacemos el cateo.
    if(bytes == 1){
        uint8_t valor_1_byte = 0;
        memcpy(&valor_1_byte, cadena, bytes);

        return (uint32_t) valor_1_byte;
    }

    if(bytes == 4){
        uint32_t valor_4_byte = 0;
        memcpy(&valor_4_byte, cadena, bytes);

        return (uint32_t) valor_4_byte;
    }

    // Copiar los bytes de la cadena al valor uint32_t 
    memcpy(&valor, cadena, bytes);

    return valor;
}

char* valor_entero_a_cadena(uint32_t entero, size_t bytes){
    /*
    if (bytes != 1 && bytes != 4) {
        return NULL;
    }
    */
    char* cadena = malloc(bytes + 1); 

    // Copiar los bytes del uint32_t a la cadena
    memcpy(cadena, &entero, bytes);

    // Agregar el carácter nulo al final de la cadena
    cadena[bytes] = '\0';

    return cadena;
}

char* valor_entero_a_cadena_nueva(uint32_t entero, size_t bytes) {
    /*
    if (bytes != 1 && bytes != 4) {
        return NULL;
    }
    */

    // Asignar memoria para la cadena incluyendo el carácter nulo
    char* cadena = malloc(bytes + 1);
    if (cadena == NULL) {
        perror("Error en malloc");
        return NULL;
    }

    // Copiar los bytes del uint32_t a la cadena
    memcpy(cadena, &entero, bytes);

    // Agregar el carácter nulo al final de la cadena
    cadena[bytes] = '\0';

    return cadena;
}


char* convertir_a_cadena(void* data, size_t bytes){
    // Reasignar memoria para agregar espacio para el carácter nulo
    char* cadena = (char*)realloc(data, bytes + 1);
    if (cadena == NULL) {
        perror("Error en realloc");
        return NULL;
    }

    // Agregar el carácter nulo al final de la cadena
    cadena[bytes] = '\0';

    /*
    Cuando usas realloc:
    Éxito: Si puede ajustar el tamaño del bloque en su ubicación actual o moverlo, devuelve un nuevo puntero. 
           Debes usar este nuevo puntero para futuras operaciones y liberación de memoria. (free(cadena))
    Fallo: Si no puede ajustar el tamaño del bloque (por falta de memoria, por ejemplo), devuelve NULL. 
           En este caso, el puntero original sigue siendo válido y no necesita ser liberado. (free(data))
    */
    
    return cadena;
}

char* convertir_a_cadena_nueva(void* data, size_t bytes){
    // Asignar memoria para la nueva cadena incluyendo el carácter nulo
    char* cadena = (char*)malloc(bytes + 1);
    if (cadena == NULL) {
        perror("Error en malloc");
        return NULL;
    }

    // Copiar los datos originales a la nueva cadena
    memcpy(cadena, data, bytes);

    // Agregar el carácter nulo al final de la cadena
    cadena[bytes] = '\0';

    return cadena;
}