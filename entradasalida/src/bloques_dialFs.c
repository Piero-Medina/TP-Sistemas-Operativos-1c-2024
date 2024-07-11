#include "bloques_dialFs.h"

void crear_archivo_de_bloques(char* path_bloques){
    int file_descriptor = open(path_bloques, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if(file_descriptor == -1){
        log_error(logger,"Error al abrir el Archivo de Bloques");
    }

    int tamanio_archivo_de_bloques = config->block_count * config->block_size; 
    if(ftruncate(file_descriptor, tamanio_archivo_de_bloques) == -1){
        log_error(logger,"Error al truncar el Archivo de Bloques");
    }

    log_debug(logger, "Archivo de bloques (%s) levantado", path_bloques);

    close(file_descriptor);
}

FILE* abrir_archivo_de_bloques(void){
    FILE* archivo_de_bloques = fopen(path_bloques, "r+b");

    if (archivo_de_bloques == NULL) {
        log_error(logger, "Archivo de bloques no existe.");
    }

    return archivo_de_bloques;
}

FILE* abrir_archivo_existente(char* path){
    FILE* archivo = fopen(path, "r+");
    if (archivo == NULL) {
        log_error(logger, "Archivo (%s) no existe", path);
    }
    return archivo;
}

FILE* crear_archivo_nuevo(char* path){
    FILE* archivo = fopen(path, "w+");
    if (archivo == NULL) {
        log_error(logger, "Archivo (%s) no se peude crear", path);
    }
    return archivo;
}

void mover_puntero_de_archivo_por_bloques(FILE* file, int posicion){
    fseek(file, (posicion * config->block_size), SEEK_SET);
}

void escribir_n_cantidad_de_caracter(FILE* file, char caracter, int cantidad){
    for (int i = 0; i < cantidad; i++) {
        fwrite(&caracter, sizeof(char), 1, file);
    }
}

void escribir_string(FILE* file, char* string){
    int tamanio_en_bytes = strlen(string);
    fwrite(string, sizeof(char), tamanio_en_bytes, file);
}

void escribir_string_n_bytes(FILE* file, char* string, int bytes){
    fwrite(string, sizeof(char), bytes, file);
}

char* leer_n_cantidad_de_caracter(FILE* file, int cantidad_bytes){
    char* buffer = malloc(sizeof(char) * (cantidad_bytes + 1));

    fread(buffer, sizeof(char), cantidad_bytes, file);

    buffer[cantidad_bytes] = '\0'; // base 0
    
    return buffer;
}

void cerrar_archivo_de_bloques(FILE* file){
    fclose(file);
}

char* crear_copia_archivo_de_bloque(void){
    FILE* original = NULL;
    FILE* copia = NULL;
    char* path_bloques_copia = NULL;

    original = abrir_archivo_de_bloques();

    path_bloques_copia = concatenar_ruta(config->path_base_dialfs, "bloques_copia.dat"); 
    crear_archivo_de_bloques(path_bloques_copia);

    copia = abrir_archivo_existente(path_bloques_copia);


    char* buffer = malloc(sizeof(char) * MAX_READ); // (4kb)
    size_t bytes;

    while ((bytes = fread(buffer, sizeof(char), MAX_READ, original)) > 0) {
        if (fwrite(buffer, sizeof(char), bytes, copia) != bytes) {
            log_error(logger, "Error escribiendo en el archivo de destino");
            cerrar_archivo_de_bloques(original);
            cerrar_archivo(copia);
            free(buffer);
            return NULL;
        }
    }

    free(buffer);
    cerrar_archivo_de_bloques(original);
    cerrar_archivo(copia);

    return path_bloques_copia;
}

void eliminar_copia_archivo_de_bloque(void){
    char* path_bloques_copia = NULL;
    path_bloques_copia = concatenar_ruta(config->path_base_dialfs, "bloques_copia.dat");

    if(remove(path_bloques_copia) != 0){
        log_error(logger,"no se pudo eliminar el archivo copia");
    }

    free(path_bloques_copia);
}
