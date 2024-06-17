#include "mmu.h"

int MMU(int direccion_logica, int tamanio_pagina, uint32_t pid, uint32_t bytes, t_list* peticiones){
    uint32_t numero_pagina = (uint32_t)floor(direccion_logica / tamanio_pagina);
    int desplazamiento = direccion_logica - numero_pagina * tamanio_pagina;

    int leer_hasta = (desplazamiento + bytes) - 1; // Asumiendo 0 desde la pagina en la que se situa.
    //int leer_hasta_absoluto = (numero_pagina * tamanio_pagina) + ((desplazamiento + bytes) - 1);
    int restante = (leer_hasta - tamanio_pagina) + 1; // 
    int paginas = (int)floor(leer_hasta / tamanio_pagina) + 1;
    int pedazito_inicial = tamanio_pagina - desplazamiento;

    printf("numero_pagina: %u\n", numero_pagina);
    printf("desplazamiento: %d\n", desplazamiento);
    printf("leer_hasta: %d\n", leer_hasta);
    //printf("leer_hasta_absoluto: %d\n", leer_hasta_absoluto);
    printf("restante: %d\n", restante);
    printf("paginas: %d\n", paginas);
    printf("pedazito_inicial: %d\n", pedazito_inicial);

    log_info(logger, "Paginas necesarias: %d", paginas);

    t_peticion_memoria* peticion = NULL;
    uint32_t direccion_fisica;
    int estado = 0;

    if(restante <= 0){
        estado = obtener_direccion_fisica(numero_pagina, pid, desplazamiento, tamanio_pagina, &direccion_fisica);
        
        if(estado != MMU_OK){
            return estado;
        }

        peticion = crear_peticion_memoria(0, direccion_fisica, bytes);
        list_add(peticiones, (void*)peticion);
        peticion = NULL;
    }
    
    if(restante > 0){
        printf("bytes iniciales: %d\n", bytes);
        int tmp = tamanio_pagina;

        if(restante < tamanio_pagina){
            tmp = tamanio_pagina - restante;
        }

        // el dsplazamiento nunca supera al tamanio de pagina
        if(pedazito_inicial != 0){
            tmp = pedazito_inicial;
        }

        estado = obtener_direccion_fisica(numero_pagina, pid, desplazamiento, tamanio_pagina, &direccion_fisica);

        if(estado != MMU_OK){
            return estado;
        }

        peticion = crear_peticion_memoria(0, direccion_fisica, tmp);
        list_add(peticiones, (void*)peticion);
        peticion = NULL;
        
        paginas -= 1;
        for(int i = 0; i < paginas; i++){
            if(restante > 0){
                printf("restante for: %d\n", restante);
                numero_pagina += 1;

                if(restante <= tamanio_pagina){
                    tmp = restante;
                }else{
                    tmp = tamanio_pagina;
                }

                // sacamos la direccion fisica de una pagina pero desde su posicion inicial
                estado = obtener_direccion_fisica(numero_pagina, pid, 0, tamanio_pagina, &direccion_fisica);

                if(estado != MMU_OK){
                    return estado;
                }

                peticion = crear_peticion_memoria(0, direccion_fisica, tmp);
                list_add(peticiones, (void*)peticion);
                peticion = NULL;
                restante -= tamanio_pagina;
                continue; 
            }

            /* la cantidad de pagnias requeridas controlan el bucle, por lo que no dara este caso
            if(restante <= 0){
                printf("restante for: %d\n", restante);
                numero_pagina += 1;
                if(restante < 0){
                    restante -= tamanio_pagina;
                    restante *= -1;
                }

                estado = obtener_direccion_fisica(numero_pagina, pid, restante, tamanio_pagina, &direccion_fisica);

                if(estado != MMU_OK){
                    return estado;
                }

                peticion = crear_peticion_memoria(0, direccion_fisica, restante);
                list_add(peticiones, (void*)peticion);
                peticion = NULL;
                break;    
            }
            */
        }
    }

    return MMU_OK;
}

int obtener_direccion_fisica(uint32_t numero_pagina, uint32_t pid, int desplazamiento, int tamanio_pagina, uint32_t* direccion_fisica){
    int32_t marco = -1;

    if(tlb_habilitada){
        marco = buscar_entrada_tlb(tlb, algoritmo_elegido, (int32_t)pid, (int32_t)numero_pagina, logger);
    }

    if(marco == -1){
        if(tlb_habilitada){
            log_warning(logger, "No se encontró entrada [Pid (%u)|Pagina (%u)] en la TLB", pid, numero_pagina);
        }

        log_info(logger, "Solicitando numero de marco a Memoria");
        envio_generico_entero(conexion_memoria, SOLICITUD_MARCO_MEMORIA, numero_pagina);

        int estado = recibo_generico_op_code(conexion_memoria);
        marco = recibo_generico_int32(conexion_memoria);
        
        if(estado == MEMORIA_OK){
            log_info(logger, "PID: <%u> - OBTENER MARCO - Página: <%u> - Marco:<%"PRId32">", pid, numero_pagina, marco);
        }
        if(estado == SEGMENTATION_FAULT){
            log_info(logger, "PID: <%u> - OBTENER MARCO - Página: <%u> - ERROR (SEGMENTATION_FAULT)", pid, numero_pagina);
            return estado;
        }

        if(tlb_habilitada){
            log_warning(logger, "Agregando entrada [Pid (%u)|Pagina (%u)|Marco (%"PRId32")] en la TLB", pid, numero_pagina, marco);
            agregar_entrada_tlb(tlb, algoritmo_elegido, (int32_t)pid, (int32_t)numero_pagina, (int32_t)marco);
        }

        *direccion_fisica = (marco * tamanio_pagina) + desplazamiento;
        return MMU_OK;

    }
    else{
        log_warning(logger, "Se encontró entrada [Pid (%u)|Pagina (%u)|Marco (%"PRId32")] en la TLB", pid, numero_pagina, marco);
        *direccion_fisica = (marco * tamanio_pagina) + desplazamiento;
        return MMU_OK;
    }
}


/*
int mmu(int direccion_logica, int tamanio_pagina, uint32_t pid, uint32_t* direccion_fisica){
    int32_t marco = -1;
    uint32_t numero_pagina = (uint32_t)floor(direccion_logica / tamanio_pagina);
    int desplazamiento = direccion_logica - numero_pagina * tamanio_pagina;

    if(tlb_habilitada){
        marco = buscar_entrada_tlb(tlb, algoritmo_elegido, (int32_t)pid, (int32_t)numero_pagina, logger);
    }

    if(marco == -1){
        if(tlb_habilitada){
            log_warning(logger, "No se encontró entrada [Pid (%u)|Pagina (%u)] en la TLB", pid, numero_pagina);
        }

        log_info(logger, "Solicitando numero de marco a Memoria");
        envio_generico_entero(conexion_memoria, SOLICITUD_MARCO_MEMORIA, numero_pagina);

        int estado = recibo_generico_op_code(conexion_memoria);
        marco = recibo_generico_int32(conexion_memoria);
        
        if(estado == MEMORIA_OK){
            log_info(logger, "PID: <%u> - OBTENER MARCO - Página: <%u> - Marco:<%"PRId32">", pid, numero_pagina, marco);
        }
        if(estado == SEGMENTATION_FAULT){
            log_info(logger, "PID: <%u> - OBTENER MARCO - Página: <%u> - ERROR (SEGMENTATION_FAULT)", pid, numero_pagina);
            return estado;
        }

        if(tlb_habilitada){
            log_warning(logger, "Agregando entrada [Pid (%u)|Pagina (%u)|Marco (%"PRId32")] en la TLB", pid, numero_pagina, marco);
            agregar_entrada_tlb(tlb, algoritmo_elegido, (int32_t)pid, (int32_t)numero_pagina, (int32_t)marco);
        }

        *direccion_fisica = (marco * tamanio_pagina) + desplazamiento;
        return MMU_OK;

    }
    else{
        log_warning(logger, "Se encontró entrada [Pid (%u)|Pagina (%u)|Marco (%"PRId32")] en la TLB", pid, numero_pagina, marco);
        *direccion_fisica = (marco * tamanio_pagina) + desplazamiento;
        return MMU_OK;
    }
}

*/