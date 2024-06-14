#include "mmu.h"

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
        if(estado == OUT_OF_MEMORY){
            log_info(logger, "PID: <%u> - OBTENER MARCO - Página: <%u> - ERROR (OUT_OF_MEMORY)", pid, numero_pagina);
            return estado;
        }

        if(tlb_habilitada){
            log_warning(logger, "Agregando entrada [Pid (%u)|Pagina (%u)|Marco (%"PRId32")] en la TLB", pid, numero_pagina, marco);
            agregar_entrada_tlb(tlb, algoritmo_elegido, (int32_t)pid, (int32_t)numero_pagina, (int32_t)marco);
        }

        *direccion_fisica = (marco * tamanio_pagina) + desplazamiento;
        return MEMORIA_OK;

    }
    else{
        log_warning(logger, "Se encontró entrada [Pid (%u)|Pagina (%u)|Marco (%"PRId32")] en la TLB", pid, numero_pagina, marco);
        *direccion_fisica = (marco * tamanio_pagina) + desplazamiento;
        return MEMORIA_OK;
    }
}
