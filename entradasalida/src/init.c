#include "init.h"

tipo_interfaz tipo_de_interfaz;

void tipo_de_interfaz_elegido(char* path){
    t_config* config_tmp = config_create(path);
    char* tipo_interfaz = config_get_string_value(config_tmp, "TIPO_INTERFAZ");

    if(strcmp(tipo_interfaz, "GENERICA") == 0) {
        tipo_de_interfaz = GENERICA;
        log_info(logger, "Tipo De Interfaz: %s \n", tipo_interfaz);
    }

    if(strcmp(tipo_interfaz,"STDIN") == 0) {
        tipo_de_interfaz = STDIN;
        log_info(logger, "Tipo De Interfaz: %s \n", tipo_interfaz);
    }

    if(strcmp(tipo_interfaz,"STDOUT") == 0) {
        tipo_de_interfaz = STDOUT;
        log_info(logger, "Tipo De Interfaz: %s \n", tipo_interfaz);
    }

    if(strcmp(tipo_interfaz,"DIALFS") == 0) {
        tipo_de_interfaz = DIALFS;
        log_info(logger, "Tipo De Interfaz: %s \n", tipo_interfaz);
    }

    config_destroy(config_tmp);
}
