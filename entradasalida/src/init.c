#include "init.h"

tipo_interfaz tipo_de_interfaz;

void tipo_de_interfaz_elegido(void){
    if(strcmp(config->tipo_interfaz, "GENERICA") == 0) {
        tipo_de_interfaz = GENERICA;
        log_info(logger, "Tipo De Interfaz: %s \n", config->tipo_interfaz);
    }

    if(strcmp(config->tipo_interfaz,"STDIN") == 0) {
        tipo_de_interfaz = STDIN;
        log_info(logger, "Tipo De Interfaz: %s \n", config->tipo_interfaz);
    }

    if(strcmp(config->tipo_interfaz,"STDOUT") == 0) {
        tipo_de_interfaz = STDOUT;
        log_info(logger, "Tipo De Interfaz: %s \n", config->tipo_interfaz);
    }

    if(strcmp(config->tipo_interfaz,"DIALFS") == 0) {
        tipo_de_interfaz = DIALFS;
        log_info(logger, "Tipo De Interfaz: %s \n", config->tipo_interfaz);
    }
}
