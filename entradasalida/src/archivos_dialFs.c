#include "archivos_dialFs.h"

estado_interfaz crear_archivo(char* nombre){
    FILE* archivo_de_bloques = NULL;
    t_archivo *archivo = NULL;
    int cant_bloques_libres, bloque_minimo, numero_bloque;

    bloque_minimo = 1; // segun el enunciado
    cant_bloques_libres = bitmap_cantidad_de_bloques_libres(bitmap);
    printf("Cantidad de bloques libres (%d) \n", cant_bloques_libres);

    // si no hay espacio para crear un archivo
    if(cant_bloques_libres < bloque_minimo){
        return SIN_ESPACIO;
    }

    // si hubo almenos un bloque libre entonces almenos hay 1 numero de bloque disponible.
    numero_bloque = bitmap_posicion_bloque_contiguo_de_n_bloques(bitmap, bloque_minimo);
    printf("numero_de_bloque_libre (%d) \n", numero_bloque);

    archivo_de_bloques = abrir_archivo_de_bloques();
    mover_puntero_de_archivo_por_bloques(archivo_de_bloques, numero_bloque);

    // crear_Archivo
    archivo = t_archivo_crear(nombre, numero_bloque);
    imprimir_t_archivo(archivo);

    crear_metadata(archivo);
    bitmap_marcar_bloque_como_ocupado(bitmap, numero_bloque);
    escribir_primer_bloque(archivo_de_bloques, archivo);

    dictionary_put(diccionario_de_archivos, nombre, (void*) archivo);

    cerrar_archivo_de_bloques(archivo_de_bloques);

    return TODO_OK;    
}

//
t_archivo* t_archivo_crear(char* nombre, int bloque_inicial){
    t_archivo* archivo = malloc(sizeof(t_archivo));

    char* nombre_sin_extension = remover_extension(nombre);
    char* nombre_metadato = concatenar_nombre_y_extension(nombre_sin_extension, ".config");
    char* ruta_metadato = concatenar_ruta(path_metadatos, nombre_metadato);
    char* ruta_archivo = concatenar_ruta(path_archivos, nombre);

    archivo->nombre = strdup(nombre);
    archivo->path_metadato = ruta_metadato;
    archivo->path_archivo = ruta_archivo;
    archivo->bloque_inicial = bloque_inicial;
    archivo->tamanio_archivo = 0;

    free(nombre_sin_extension);
    free(nombre_metadato);

    return archivo;
}

void imprimir_t_archivo(t_archivo* archivo){
    if (archivo == NULL) {
        printf("El archivo es NULL.\n");
        return;
    }

    printf("Nombre: %s\n", archivo->nombre);
    printf("Ruta del archivo: %s\n", archivo->path_archivo);
    printf("Ruta del metadato: %s\n", archivo->path_metadato);
    printf("Bloque inicial: %d\n", archivo->bloque_inicial);
    printf("Tamaño del archivo: %d bytes\n\n", archivo->tamanio_archivo);
}

void liberar_elemento_t_archivo(void* archivo){
    t_archivo* tmp = (t_archivo*) archivo;
    
    free(tmp->nombre);
    free(tmp->path_metadato);
    free(tmp->path_archivo);
    free(tmp);
}

void crear_metadata(t_archivo* archivo){
    FILE* archivo_de_metadato = NULL;

    archivo_de_metadato = crear_archivo_nuevo(archivo->path_metadato);
    if (archivo_de_metadato == NULL) {
        log_error(logger, "Error al crear el Archivo Metadato del Archivo (%s).", archivo->nombre);
        exit(EXIT_FAILURE);
    }

    fprintf(archivo_de_metadato,"BLOQUE_INICIAL=%d\n", archivo->bloque_inicial);
    fprintf(archivo_de_metadato,"TAMANIO_ARCHIVO=%d", archivo->tamanio_archivo);

    fclose(archivo_de_metadato);
}

void escribir_primer_bloque(FILE* archivo_de_bloques, t_archivo* archivo){
    FILE* archivo_fisico = NULL;
    char vacio_basura = 'F';

    // solo creamos el archivo
    archivo_fisico = crear_archivo_nuevo(archivo->path_archivo);
    //archivo_fisico = fopen(archivo->path_archivo, "w+");
    if (archivo_fisico == NULL) {
        log_error(logger, "Error al crear el Archivo Fisico del Archivo (%s).", archivo->nombre);
        exit(EXIT_FAILURE);
    }

    // archivo fisico solo se crear

    // escribir archivo de bloques
    mover_puntero_de_archivo_por_bloques(archivo_de_bloques, archivo->bloque_inicial);
    escribir_n_cantidad_de_caracter(archivo_de_bloques, vacio_basura, config->block_size);

    fclose(archivo_fisico);
}

void levantar_diccionario_de_archivos(t_dictionary* archivos){
    DIR *dir;
    struct dirent *ent;
    t_archivo* tmp = NULL;
    int hay_archivos_validos = 0;
    
    // Intentar abrir el directorio
    if ((dir = opendir(path_metadatos)) == NULL) {
        log_error(logger, "No se pudo abrir el directorio (%s)", path_metadatos);
        exit(EXIT_FAILURE);
    }

    //*******
    // Verificar si hay archivos válidos en el directorio
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
            hay_archivos_validos = 1;
            break;
        }
    }

    // Si no hay archivos válidos, cerrar el directorio y salir de la función
    if (!hay_archivos_validos) {
        closedir(dir);
        log_info(logger, "El directorio (%s) no contiene archivos válidos", path_metadatos);
        return;
    }

    // Volver al inicio del directorio y procesar los archivos
    rewinddir(dir);
    //*****

    // El directorio se abrió correctamente, iterar sobre los elementos
    while ((ent = readdir(dir)) != NULL) {
        // Ignorar los directorios . y ..
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
            tmp = levantar_t_archivo(ent->d_name);
            dictionary_put(archivos, tmp->nombre, (void*) tmp);
            log_warning(logger, "Archivo (%s) Recuperado", tmp->nombre);
        }
    }
    
    closedir(dir);
}

t_archivo* levantar_t_archivo(char* nombre_extension_metadato){
    t_config* config_metadato = NULL;
    t_archivo* archivo = malloc(sizeof(t_archivo));

    char* nombre_sin_extension = remover_extension(nombre_extension_metadato);

    char* nombre_con_extension = concatenar_nombre_y_extension(nombre_sin_extension, ".txt");
    char* ruta_metadato = concatenar_ruta(path_metadatos, nombre_extension_metadato);
    char* ruta_archivo = concatenar_ruta(path_archivos, nombre_con_extension);

    config_metadato = config_create(ruta_metadato);
    int bloque_inicial = config_get_int_value(config_metadato, "BLOQUE_INICIAL");
    int tamanio_archivo = config_get_int_value(config_metadato, "TAMANIO_ARCHIVO");

    archivo->nombre = nombre_con_extension;
    archivo->path_metadato = ruta_metadato;
    archivo->path_archivo = ruta_archivo;
    archivo->bloque_inicial = bloque_inicial;
    archivo->tamanio_archivo = tamanio_archivo;

    config_destroy(config_metadato);
    free(nombre_sin_extension);

    return archivo;
}

estado_interfaz truncar_archivo(char* nombre, int tamanio_nuevo, int pid){
    log_info(logger, "F - truncar_archivo \n");
    int cant_bloques_libres, diferencia;
    int tamanio_en_bloques_original, tamanio_en_bloques_nuevo;
    t_archivo* archivo = NULL;

    archivo = (t_archivo*) dictionary_get(diccionario_de_archivos, nombre);

    // evaluamos esto antes asi no abrimos nada si se da el caso.
    if(archivo->tamanio_archivo == tamanio_nuevo){
        return NO_PROCESAR;
    }

    diferencia = tamanio_nuevo - archivo->tamanio_archivo;
    cant_bloques_libres = bitmap_cantidad_de_bloques_libres(bitmap);

    // no hay bloques libres suficientes para truncar
    if(diferencia > cant_bloques_libres){
        return SIN_ESPACIO;
    }

    tamanio_en_bloques_original = cantidad_de_bloques_ocupados(archivo->tamanio_archivo, config->block_size);
    log_info(logger, "tamanio_en_bloques_original (%d) \n", tamanio_en_bloques_original);
    tamanio_en_bloques_nuevo = cantidad_de_bloques_ocupados(tamanio_nuevo, config->block_size);
    log_info(logger, "tamanio_en_bloques_nuevo (%d) \n", tamanio_en_bloques_nuevo);

    if(tamanio_en_bloques_original < tamanio_en_bloques_nuevo){
        log_info(logger, "Toca ampliar \n");
        ampliar_archivo(archivo, tamanio_nuevo, config->block_size, pid);
    } 
    if(tamanio_en_bloques_original > tamanio_en_bloques_nuevo){
        log_info(logger, "Toca reducir \n");
        reducir_archivo(archivo, tamanio_nuevo, config->block_size); 
    }
    if(tamanio_en_bloques_original == tamanio_en_bloques_nuevo){
        log_info(logger, "Toca magia \n");
        diferenciar_de_magia_archivo(archivo, tamanio_nuevo, config->block_size);
    }

    return TODO_OK;    
}

//
void reducir_archivo(t_archivo* archivo, int nuevo_tamanio_bytes, int tamanio_bloque){
    log_info(logger, "F - reducir_archivo \n");
    int tamanio_en_bloques_original, tamanio_en_bloques_nuevo, diferencia_de_bloques;
    int bloque_archivo_fisico_final_absoluto_nuevo, bloque_archivo_final_absoluto_nuevo; 
    //int bloque_archivo_final_absoluto_actual;
    FILE* archivo_de_bloques = NULL, *archivo_fisico = NULL;
    t_config* metadato = NULL;
    char vacio = '0';
    char vacio_fisico = '0';

    if(nuevo_tamanio_bytes == 0){
        log_info(logger, "Toca vaciar archivo \n");
        vaciar_archivo(archivo);
        return;
    }

    tamanio_en_bloques_original = cantidad_de_bloques_ocupados(archivo->tamanio_archivo, config->block_size);
    tamanio_en_bloques_nuevo = cantidad_de_bloques_ocupados(nuevo_tamanio_bytes, config->block_size);
    diferencia_de_bloques = tamanio_en_bloques_original - tamanio_en_bloques_nuevo;

    archivo_de_bloques = abrir_archivo_de_bloques();
    metadato = config_create(archivo->path_metadato);
    archivo_fisico = abrir_archivo_existente(archivo->path_archivo);

    //bloque_archivo_final_absoluto_actual = archivo->bloque_inicial + tamanio_en_bloques_original;
    bloque_archivo_final_absoluto_nuevo = archivo->bloque_inicial + tamanio_en_bloques_nuevo;
    // nos situamos en el primer byte del bloque que ya no nos perteneces |m|m|m|no es nuestro, de aca emepzamos|
    fseek(archivo_de_bloques, (bloque_archivo_final_absoluto_nuevo + 1) * config->block_size, SEEK_SET);
     
    bloque_archivo_fisico_final_absoluto_nuevo = (tamanio_en_bloques_original - diferencia_de_bloques) - 1;
    // nos situamos en el primer byte del bloque que no no nos peretenece
    fseek(archivo_fisico, ( (bloque_archivo_fisico_final_absoluto_nuevo + 1) * config->block_size), SEEK_SET);

    bitmap_marcar_bloques_libres_desde_posicion(bitmap, (bloque_archivo_final_absoluto_nuevo + 1), tamanio_en_bloques_original);
    
    escribir_n_cantidad_de_caracter(archivo_de_bloques, vacio, (config->block_size * diferencia_de_bloques) );
    escribir_n_cantidad_de_caracter(archivo_fisico, vacio_fisico, (config->block_size * diferencia_de_bloques) );
    
    archivo->tamanio_archivo = nuevo_tamanio_bytes;
    set_key_metadata(metadato, "TAMANIO_ARCHIVO", archivo->tamanio_archivo);

    cerrar_archivo_de_bloques(archivo_de_bloques);
    config_destroy(metadato); 
    cerrar_archivo(archivo_fisico); 
}

void vaciar_archivo(t_archivo* archivo){
    log_info(logger, "F - vaciar_archivo \n");
    FILE* archivo_de_bloques = NULL, *archivo_fisico = NULL;
    t_config* metadata = NULL;
    int bloque_inicial_relativo, tamanio_en_bloques_original;
    char vacio = '0';
    char vacio_fisico = '0';

    archivo_de_bloques = abrir_archivo_de_bloques();
    metadata = config_create(archivo->path_metadato);
    archivo_fisico = abrir_archivo_existente(archivo->path_archivo);

    // no cuentan el bloque Inicial del archivo (bloque que ocupa por el hecho de ser creado);
    bloque_inicial_relativo = archivo->bloque_inicial + 1;
    log_info(logger, "bloque_inicial_relativo (%d) \n", bloque_inicial_relativo);
    tamanio_en_bloques_original = cantidad_de_bloques_ocupados(archivo->tamanio_archivo, config->block_size);
    log_info(logger, "tamanio_en_bloques_original (%d) \n", tamanio_en_bloques_original);

    log_info(logger, "puntero_posicion_a_escribir_archivo (%d) \n", (bloque_inicial_relativo * config->block_size));
    mover_puntero_de_archivo_por_bloques(archivo_de_bloques, bloque_inicial_relativo);
    mover_puntero_de_archivo_por_bloques(archivo_fisico, 0);

    log_info(logger, "bloque_inicial_relativo (%d) | tamanio_en_bloques_original (%d) \n", bloque_inicial_relativo, tamanio_en_bloques_original);
    bitmap_marcar_bloques_libres_desde_posicion(bitmap, bloque_inicial_relativo, tamanio_en_bloques_original);

    int cantidad_de_caracteres_vacios_a_escribir = (config->block_size * tamanio_en_bloques_original);
    log_info(logger, "cantidad_de_caracteres_vacios_a_escribir (%d) \n", cantidad_de_caracteres_vacios_a_escribir);
    escribir_n_cantidad_de_caracter(archivo_de_bloques, vacio, (config->block_size * tamanio_en_bloques_original) );
    escribir_n_cantidad_de_caracter(archivo_fisico, vacio_fisico, (config->block_size * tamanio_en_bloques_original));

    archivo->tamanio_archivo = 0;
    set_key_metadata(metadata, "TAMANIO_ARCHIVO", archivo->tamanio_archivo);

    cerrar_archivo_de_bloques(archivo_de_bloques);
    config_destroy(metadata); 
    cerrar_archivo(archivo_fisico);   
}

void ampliar_archivo(t_archivo* archivo, int nuevo_tamanio_bytes, int tamanio_bloque, int pid){
    int tamanio_en_bloques_original, tamanio_en_bloques_nuevo, diferencia_de_bloques;
    int bloque_archivo_final_absoluto_original, cant_bloques_contiguos;

    tamanio_en_bloques_original = cantidad_de_bloques_ocupados(archivo->tamanio_archivo, config->block_size);
    tamanio_en_bloques_nuevo = cantidad_de_bloques_ocupados(nuevo_tamanio_bytes, config->block_size);
    diferencia_de_bloques = tamanio_en_bloques_nuevo - tamanio_en_bloques_original;

    bloque_archivo_final_absoluto_original = archivo->bloque_inicial + tamanio_en_bloques_original;
    log_info(logger, "bloque_archivo_final_absoluto_original (%d) \n", bloque_archivo_final_absoluto_original);

    cant_bloques_contiguos = bitmap_cant_bloques_libres_contiguos_desde_posicion(bitmap, diferencia_de_bloques, (bloque_archivo_final_absoluto_original + 1));
    log_info(logger, "cant_bloques_contiguos (%d) \n", cant_bloques_contiguos);

    if(cant_bloques_contiguos == diferencia_de_bloques){
        sub_ampliar_archivo(archivo, nuevo_tamanio_bytes);
        return;
    }

    log_info(logger, "PID: <%d> - Inicio Compactacion", pid);
    char* nombre_archivo = archivo->nombre;
    compactar_archivos_usando_mas_memoria(diccionario_de_archivos, nombre_archivo);
    log_info(logger, "PID: <%d> - Fin Compactacion", pid);

    // verificando
    archivo = (t_archivo*) dictionary_get(diccionario_de_archivos, nombre_archivo);
    
    bloque_archivo_final_absoluto_original = archivo->bloque_inicial + tamanio_en_bloques_original;

    cant_bloques_contiguos = bitmap_cant_bloques_libres_contiguos_desde_posicion(bitmap, diferencia_de_bloques, (bloque_archivo_final_absoluto_original + 1));

    if(cant_bloques_contiguos == diferencia_de_bloques){
        sub_ampliar_archivo(archivo, nuevo_tamanio_bytes);
        return;
    }
}

void sub_ampliar_archivo(t_archivo * archivo, int nuevo_tamanio_bytes){
    log_info(logger, "F - sub_ampliar_archivo \n");
    int tamanio_en_bloques_original, tamanio_en_bloques_nuevo, diferencia_de_bloques;
    int bloque_archivo_final_absoluto_original;
    FILE* archivo_de_bloques = NULL;
    t_config* metadato = NULL;
    char permitido = '1';

    archivo_de_bloques = abrir_archivo_de_bloques();
    metadato = config_create(archivo->path_metadato);

    tamanio_en_bloques_original = cantidad_de_bloques_ocupados(archivo->tamanio_archivo, config->block_size);
    tamanio_en_bloques_nuevo = cantidad_de_bloques_ocupados(nuevo_tamanio_bytes, config->block_size);
    diferencia_de_bloques = tamanio_en_bloques_nuevo - tamanio_en_bloques_original;

    bloque_archivo_final_absoluto_original = archivo->bloque_inicial + tamanio_en_bloques_original;
    log_info(logger, "bloque_archivo_final_absoluto_original (%d) \n", bloque_archivo_final_absoluto_original);


    int puntero_posicion_a_escribir = (bloque_archivo_final_absoluto_original + 1) * config->block_size;
    log_info(logger, "puntero_posicion_a_escribir (%d) \n", puntero_posicion_a_escribir);
    int cantidad_de_caracteres_vacios_a_escribir = (config->block_size * diferencia_de_bloques);
    log_info(logger, "cantidad_de_caracteres_vacios_a_escribir (%d) \n", cantidad_de_caracteres_vacios_a_escribir);

    fseek(archivo_de_bloques, (bloque_archivo_final_absoluto_original + 1) * config->block_size, SEEK_SET);
    escribir_n_cantidad_de_caracter(archivo_de_bloques, permitido, (config->block_size * diferencia_de_bloques) );

    archivo->tamanio_archivo = nuevo_tamanio_bytes;
    set_key_metadata(metadato, "TAMANIO_ARCHIVO", archivo->tamanio_archivo);

    int bloque_inicio_a_marcar_como_ocupado = (bloque_archivo_final_absoluto_original + 1);
    log_info(logger, "bloque_inicio_a_marcar_como_ocupado (%d) | cuantos bloques (%d) \n", bloque_inicio_a_marcar_como_ocupado, diferencia_de_bloques);
    bitmap_marcar_bloques_ocupados_desde_posicion(bitmap, (bloque_archivo_final_absoluto_original + 1), diferencia_de_bloques);

    config_destroy(metadato);
    cerrar_archivo_de_bloques(archivo_de_bloques);

}

void diferenciar_de_magia_archivo(t_archivo* archivo, int nuevo_tamanio_bytes, int tamanio_bloque){
    int diferencia_en_bytes, fragmentacion_interna, tamanio_bloques_original, bytes_magicos, posicion_puntero;
    int bloque_archivo_final_absoluto_original, bloque_archivo_fisico_final_original;
    char vacio = '0';
    char permitido = '1';
    FILE* archivo_de_bloques = NULL, *archivo_fisico = NULL;
    t_config* metadato = NULL;

    archivo_de_bloques = abrir_archivo_de_bloques();
    metadato = config_create(archivo->path_metadato);
    archivo_fisico = abrir_archivo_existente(archivo->path_archivo);
    
    diferencia_en_bytes = archivo->tamanio_archivo - nuevo_tamanio_bytes;
    fragmentacion_interna = bytes_sobrantes_de_ultimo_bloque(archivo->tamanio_archivo, tamanio_bloque);
    tamanio_bloques_original = cantidad_de_bloques_ocupados(archivo->tamanio_archivo, tamanio_bloque);

    // bytes escribibles del ultimo bloque
    bytes_magicos = tamanio_bloque - fragmentacion_interna;
    
    // ultimo_bloque_escribible
    bloque_archivo_final_absoluto_original = archivo->bloque_inicial + tamanio_bloques_original;

    // ultimo_bloque_escribible_fisico
    bloque_archivo_fisico_final_original = tamanio_bloques_original - 1;

    // diferencia positiva
    if(archivo->tamanio_archivo > nuevo_tamanio_bytes){
        // reducir bytes
        posicion_puntero =((bloque_archivo_final_absoluto_original * tamanio_bloque) + bytes_magicos) - diferencia_en_bytes; 
        fseek(archivo_de_bloques, posicion_puntero, SEEK_SET);
        escribir_n_cantidad_de_caracter(archivo_de_bloques, vacio, diferencia_en_bytes);

        posicion_puntero =((bloque_archivo_fisico_final_original * tamanio_bloque) + bytes_magicos) - diferencia_en_bytes;
        fseek(archivo_fisico, posicion_puntero, SEEK_SET);
        escribir_n_cantidad_de_caracter(archivo_fisico, vacio, diferencia_en_bytes); 
    }

    // diferencia negativa
    if(archivo->tamanio_archivo < nuevo_tamanio_bytes){
        // ampliar bytes
       diferencia_en_bytes *= -1;
       posicion_puntero =((bloque_archivo_final_absoluto_original * tamanio_bloque) + bytes_magicos); 
       fseek(archivo_de_bloques, posicion_puntero, SEEK_SET);
       escribir_n_cantidad_de_caracter(archivo_de_bloques, permitido, diferencia_en_bytes);

       posicion_puntero =((bloque_archivo_fisico_final_original * tamanio_bloque) + bytes_magicos);
       fseek(archivo_fisico, posicion_puntero, SEEK_SET);
       escribir_n_cantidad_de_caracter(archivo_fisico, permitido, diferencia_en_bytes); 
    }

    archivo->tamanio_archivo = nuevo_tamanio_bytes;
    set_key_metadata(metadato, "TAMANIO_ARCHIVO", archivo->tamanio_archivo);

    cerrar_archivo_de_bloques(archivo_de_bloques);
    config_destroy(metadato); 
    cerrar_archivo(archivo_fisico);
}


void compactar_archivos(t_dictionary* archivos, char* nombre_archivo){
    t_list* lista = dictionary_elements(archivos);
    t_link_element **indirect = &lista->head;

    t_archivo* tmp = NULL;
    FILE * archivo_de_bloques = NULL;
    t_config* metada = NULL;
    FILE * archivo_fisico = NULL;
    char* buffer = malloc(sizeof(char) * MAX_READ);
    char* temporal = NULL;
    int puntero_archivo_posicion = 0;
    int tamanio_total_en_bloques = 0; // contando el byte basura
    int tamanio_en_bytes = 0;
    char vacio_fisico = ' ';
    char vacio = '0';
    int tamanio_total_en_bloques_test = 0;
    int fragmentacion_interna = 0;
    int desde_donde_escribir = 0;

    archivo_de_bloques = abrir_archivo_de_bloques();

    while ((*indirect) != NULL) {
		tmp = (t_archivo*) (*indirect)->data;
        
        if(strcmp(tmp->nombre, nombre_archivo) == 0){
            // nada, lo dejamos para lo ultimo
            indirect = &(*indirect)->next;
            continue;
        }

        metada = config_create(tmp->path_metadato);
        archivo_fisico = abrir_archivo_existente(tmp->path_archivo);

        // lee hasta que halla un salto de linea (no sabemos si en el tp mandaran saltos de linea)
        fgets(buffer, MAX_READ, archivo_fisico);

        // INIT COMPACT
        temporal = eliminar_caracteres_finales(buffer, vacio_fisico);
        tamanio_en_bytes = strlen(temporal);

        tamanio_total_en_bloques = cantidad_de_bloques_ocupados((tmp->tamanio_archivo + config->block_size), config->block_size);

        // inicio pequenio Test
        tamanio_total_en_bloques_test = cantidad_de_bloques_ocupados((tamanio_en_bytes + config->block_size), config->block_size);
        if(tamanio_total_en_bloques == tamanio_total_en_bloques_test){
            printf("Coinciden bloques \n");
        }
        // fin pequenio Test
        
        // recordemos que hay que llenar el primero bloque con 0000 
        fseek(archivo_de_bloques, puntero_archivo_posicion * config->block_size, SEEK_SET);
        escribir_n_cantidad_de_caracter(archivo_de_bloques, vacio, config->block_size);
        //fwrite(&vacio, sizeof(char), config->block_size, archivo_de_bloques);

        // habiendo llenado el bloque con 0000 nos movemos al siguiente bloque en donde empieza el contenido y escribimos
        fseek(archivo_de_bloques, (puntero_archivo_posicion * config->block_size) + (config->block_size), SEEK_SET);
        escribir_string_n_bytes(archivo_de_bloques, buffer, tamanio_en_bytes);
        //fwrite(buffer, sizeof(char), tamanio_en_bytes, archivo_de_bloques);

        // podemos calcular la fragmentacion interna y retrocerder un bloque (del final) y escribir con 000 lo que es fragmentacion
        fragmentacion_interna = bytes_sobrantes_de_ultimo_bloque((tmp->tamanio_archivo + config->block_size), config->block_size);
        if(fragmentacion_interna != 0){
            desde_donde_escribir = config->block_size - fragmentacion_interna;
            // me situo en el inicio del ultimo bloque 
            fseek(archivo_de_bloques, (puntero_archivo_posicion * config->block_size) + (tamanio_total_en_bloques) , SEEK_SET);
            // me corro en el bloque los ultimos bytes disponibles del archivo y empiezo a llenar con basura la fragmentacion interna
            fseek(archivo_de_bloques, desde_donde_escribir, SEEK_CUR);
            escribir_n_cantidad_de_caracter(archivo_de_bloques, vacio, fragmentacion_interna);
            //fwrite(&vacio, sizeof(char), fragmentacion_interna, archivo_de_bloques); 
        }

        tmp->bloque_inicial = puntero_archivo_posicion;
        set_key_metadata(metada, "BLOQUE_INICIAL", tmp->tamanio_archivo);

        bitmap_marcar_bloques_ocupados_desde_posicion(bitmap, puntero_archivo_posicion, (tmp->tamanio_archivo + 1));
        
        puntero_archivo_posicion += tamanio_total_en_bloques;
        // FIN COMPACT

        // limpiando todo
        config_destroy(metada);
        cerrar_archivo(archivo_fisico);
        
        free(temporal);
        temporal = NULL;
        free(buffer);
        buffer = malloc(sizeof(char) * MAX_READ);

        metada = NULL;
        archivo_fisico = NULL;

		indirect = &(*indirect)->next;
	}

    // moviendo el archivo al final (se repite logica pero solo es un caso)
    tmp = (t_archivo*) dictionary_get(archivos, nombre_archivo);

    metada = config_create(tmp->path_metadato);
    archivo_fisico = abrir_archivo_existente(tmp->path_archivo);

    // lee hasta que halla un salto de linea (no sabemos si en el tp mandaran saltos de linea)
    fgets(buffer, MAX_READ, archivo_fisico);

    // INIT COMPACT
    temporal = eliminar_caracteres_finales(buffer, vacio_fisico);
    tamanio_en_bytes = strlen(temporal);

    tamanio_total_en_bloques = cantidad_de_bloques_ocupados((tmp->tamanio_archivo + config->block_size), config->block_size);

    // inicio pequenio Test
    tamanio_total_en_bloques_test = cantidad_de_bloques_ocupados((tamanio_en_bytes + config->block_size), config->block_size);
    if(tamanio_total_en_bloques == tamanio_total_en_bloques_test){
        printf("Coinciden bloques \n");
    }
    // fin pequenio Test
        
    // recordemos que hay que llenar el primero bloque con 0000 
    fseek(archivo_de_bloques, puntero_archivo_posicion * config->block_size, SEEK_SET);
    escribir_n_cantidad_de_caracter(archivo_de_bloques, vacio, config->block_size);
    //fwrite(&vacio, sizeof(char), config->block_size, archivo_de_bloques);

    // habiendo llenado el bloque con 0000 nos movemos al siguiente bloque en donde empieza el contenido y escribimos
    fseek(archivo_de_bloques, (puntero_archivo_posicion * config->block_size) + (config->block_size), SEEK_SET);
    escribir_string_n_bytes(archivo_de_bloques, buffer, tamanio_en_bytes);
    //fwrite(buffer, sizeof(char), tamanio_en_bytes, archivo_de_bloques);

    // podemos calcular la fragmentacion interna y retrocerder un bloque (del final) y escribir con 000 lo que es fragmentacion
    fragmentacion_interna = bytes_sobrantes_de_ultimo_bloque((tmp->tamanio_archivo + config->block_size), config->block_size);
    if(fragmentacion_interna != 0){
        desde_donde_escribir = config->block_size - fragmentacion_interna;
        // me situo en el inicio del ultimo bloque 
        fseek(archivo_de_bloques, (puntero_archivo_posicion * config->block_size) + (tamanio_total_en_bloques) , SEEK_SET);
        // me corro en el bloque los ultimos bytes disponibles del archivo y empiezo a llenar con basura la fragmentacion interna
        fseek(archivo_de_bloques, desde_donde_escribir, SEEK_CUR);
        escribir_n_cantidad_de_caracter(archivo_de_bloques, vacio, fragmentacion_interna);
        //fwrite(&vacio, sizeof(char), fragmentacion_interna, archivo_de_bloques); 
    }

    tmp->bloque_inicial = puntero_archivo_posicion;
    set_key_metadata(metada, "BLOQUE_INICIAL", tmp->tamanio_archivo);

    bitmap_marcar_bloques_ocupados_desde_posicion(bitmap, puntero_archivo_posicion, (tmp->tamanio_archivo + 1));

    // limpiando todo
    config_destroy(metada);
    cerrar_archivo(archivo_fisico);
        
    free(temporal);
    free(buffer);
    
    list_destroy(lista);

    cerrar_archivo_de_bloques(archivo_de_bloques);
}

void compactar_archivos_usando_mas_memoria(t_dictionary* archivos, char* nombre_archivo){
    t_list* lista = dictionary_elements(archivos);
    t_link_element **indirect = &lista->head;
    t_archivo* tmp = NULL;

    FILE* archivo_de_bloques = NULL;
    FILE* copia = NULL;
    char* path_copia = NULL;
    t_config* metadato = NULL;
    char* buffer = malloc(sizeof(char) * MAX_READ);
    char* test = NULL;
    int puntero_archivo_posicion = 0;
    int tamanio_en_bloques_total = 0;
    int tamanio_en_bytes_total = 0;

    path_copia = crear_copia_archivo_de_bloque();
    archivo_de_bloques = abrir_archivo_de_bloques();
    copia = abrir_archivo_existente(path_copia);

    while ((*indirect) != NULL) {
		tmp = (t_archivo*) (*indirect)->data;
        
        if(strcmp(tmp->nombre, nombre_archivo) == 0){
            // nada, lo dejamos para lo ultimo
            indirect = &(*indirect)->next;
            continue;
        }

        metadato = config_create(tmp->path_metadato);

        tamanio_en_bloques_total = cantidad_de_bloques_ocupados((tmp->tamanio_archivo + config->block_size), config->block_size);
        tamanio_en_bytes_total = tmp->tamanio_archivo + config->block_size;

        // se situa en el inicio del archivo
        fseek(copia, (tmp->bloque_inicial * config->block_size), SEEK_SET);
        fread(buffer, sizeof(char), tamanio_en_bytes_total, copia);

        fseek(copia, (tmp->bloque_inicial * config->block_size), SEEK_SET);
        test = leer_n_cantidad_de_caracter(copia, tamanio_en_bytes_total);
        printf(" Se Leyo - bytes (%d) buffer (%s) \n",tamanio_en_bytes_total, test);
        free(test);
        test = NULL;

        printf(" Desde donde escribimos en el archivo de bloques - bytes (%d)  \n",(puntero_archivo_posicion * config->block_size));
        fseek(archivo_de_bloques, (puntero_archivo_posicion * config->block_size), SEEK_SET);
        fwrite(buffer, sizeof(char), tamanio_en_bytes_total, archivo_de_bloques);

        printf("Bitmap marcar bloques libres - desde (%d) - cuanto (%d) \n", tmp->bloque_inicial, tamanio_en_bloques_total);
        bitmap_marcar_bloques_libres_desde_posicion(bitmap, tmp->bloque_inicial, tamanio_en_bloques_total);

        tmp->bloque_inicial = puntero_archivo_posicion;
        set_key_metadata(metadato, "BLOQUE_INICIAL", tmp->bloque_inicial);

        printf("Bitmap marcar bloques ocupados - desde (%d) - cuanto (%d) \n", tmp->bloque_inicial, tamanio_en_bloques_total);
        bitmap_marcar_bloques_ocupados_desde_posicion(bitmap, tmp->bloque_inicial, tamanio_en_bloques_total);

        puntero_archivo_posicion += tamanio_en_bloques_total;
		indirect = &(*indirect)->next;

        // liberacion
        config_destroy(metadato);
        metadato = NULL;
	}

    // ultimo archivo
    tmp = (t_archivo*) dictionary_get(archivos, nombre_archivo);
    metadato = config_create(tmp->path_metadato);

    tamanio_en_bloques_total = cantidad_de_bloques_ocupados((tmp->tamanio_archivo + config->block_size), config->block_size);
    tamanio_en_bytes_total = tmp->tamanio_archivo + config->block_size;

    // 
    fseek(copia, (tmp->bloque_inicial * config->block_size), SEEK_SET);
    fread(buffer, sizeof(char), tamanio_en_bytes_total, copia);

    fseek(copia, (tmp->bloque_inicial * config->block_size), SEEK_SET);
    test = leer_n_cantidad_de_caracter(copia, tamanio_en_bytes_total);
    printf(" Se Leyo - bytes (%d) buffer (%s) \n",tamanio_en_bytes_total, test);
    free(test);
    test = NULL;

    printf(" Desde donde escribimos en el archivo de bloques (%d) \n",(puntero_archivo_posicion * config->block_size));
    fseek(archivo_de_bloques, (puntero_archivo_posicion * config->block_size), SEEK_SET);
    fwrite(buffer, sizeof(char), tamanio_en_bytes_total, archivo_de_bloques);

    tmp->bloque_inicial = puntero_archivo_posicion;
    set_key_metadata(metadato, "BLOQUE_INICIAL", tmp->bloque_inicial);

    printf("Bitmap marcar bloques ocupados - desde (%d) - cuanto (%d) \n", tmp->bloque_inicial, tamanio_en_bloques_total);
    bitmap_marcar_bloques_ocupados_desde_posicion(bitmap, tmp->bloque_inicial, tamanio_en_bloques_total);

    // liberacion
    config_destroy(metadato);


    // liberacion total
    free(path_copia);
    free(buffer);

    cerrar_archivo(copia);
    cerrar_archivo_de_bloques(archivo_de_bloques);
    
    eliminar_copia_archivo_de_bloque();
    list_destroy(lista);
}

void set_key_metadata(t_config* metadato, char* key, int valor){
    char* nuevo_tamanio = entero_a_string(valor);
    config_set_value(metadato, key, nuevo_tamanio);
    config_save(metadato);
    free(nuevo_tamanio);
}

estado_interfaz eliminar_archivo(char* nombre){
    t_archivo* archivo = NULL;
    FILE* archivo_de_bloques = NULL;
    char vacio = '0';

    archivo = (t_archivo*) dictionary_remove(diccionario_de_archivos, nombre);
    if(archivo == NULL){
        return ARCHIVO_NO_EXISTE;
    }

    archivo_de_bloques = abrir_archivo_de_bloques();

    vaciar_archivo(archivo);
    
    mover_puntero_de_archivo_por_bloques(archivo_de_bloques, archivo->bloque_inicial);
    escribir_n_cantidad_de_caracter(archivo_de_bloques, vacio, config->block_size);
    bitmap_marcar_bloque_como_libre(bitmap, archivo->bloque_inicial);

    remove(archivo->path_metadato);
    remove(archivo->path_archivo);
    
    liberar_elemento_t_archivo((void*) archivo);

    cerrar_archivo_de_bloques(archivo_de_bloques);
    
    return TODO_OK;
}
