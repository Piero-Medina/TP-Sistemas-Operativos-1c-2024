#include "consola_interactiva.h"

void consola_interactiva(void){
    char* leido = NULL;

    menu();
    leido = readline("> ");
    procesar_opcion(leido);

    while ((leido != NULL) && (strlen(leido) != 0)){
        if (*leido) {
            add_history(leido);
        }
        
        free(leido);
        leido = NULL;
        //menu();
        leido = readline("> ");
        procesar_opcion(leido);
    }
    
    if (leido != NULL) {
        free(leido);
    }

    clear_history(); 
}

void menu(void){

    printf("\nCamandos Disponibles: \n\n");

    printf("Ejecutar Script de Instrucciones: EJECUTAR_SCRIPT [PATH]\n");
    printf("Iniciar proceso: INICIAR_PROCESO [PATH]\n");
    printf("Finalizar proceso: FINALIZAR_PROCESO [PID]\n");
    printf("Detener planificación: DETENER_PLANIFICACION\n");
    printf("Iniciar planificación: INICIAR_PLANIFICACION\n");
    printf("Modificar el grado de multiprogramación: MULTIPROGRAMACION [VALOR]\n");
    printf("Listar procesos por estado: PROCESO_ESTADO\n\n");

    printf("Ingrese el comando de la opción deseada: \n");
}

void procesar_opcion(char* leido){
    t_opcion_consola opcion = opcion_seleccionada(leido);
    
    if(opcion.opcion != OPCION_NEGADA){
        ejecutar_opcion(opcion, leido);
    }else{
        printf("OPCION NO DISPONIBLE\n");
    }
}

t_opcion_consola opcion_seleccionada(char* leido){
    
    t_opcion_consola tmp;
    char** split = NULL;
    split = string_split(leido, " ");

    if(strcmp(split[0], "EJECUTAR_SCRIPT") == 0){
		tmp.opcion = EJECUTAR_SCRIPT;
        tmp.cant_parametros = 1;
	}
	else if(strcmp(split[0], "INICIAR_PROCESO") == 0){
		tmp.opcion = INICIAR_PROCESO;
        tmp.cant_parametros = 1;
	}
	else if(strcmp(split[0], "FINALIZAR_PROCESO") == 0){
        tmp.opcion = FINALIZAR_PROCESO;
        tmp.cant_parametros = 1;
	}
	else if(strcmp(split[0], "DETENER_PLANIFICACION") == 0){
		tmp.opcion = DETENER_PLANIFICACION;
        tmp.cant_parametros = 0;
	}
	else if(strcmp(split[0], "INICIAR_PLANIFICACION") == 0){
		tmp.opcion = INICIAR_PLANIFICACION;
        tmp.cant_parametros = 0;
	}
    else if(strcmp(split[0], "MULTIPROGRAMACION") == 0){
		tmp.opcion = MODIFICAR_MULTIPROGRAMACION;
        tmp.cant_parametros = 1;
	}
    else if(strcmp(split[0], "PROCESO_ESTADO") == 0){
		tmp.opcion = PROCESO_ESTADO;
        tmp.cant_parametros = 0;
	}else{
        tmp.opcion = OPCION_NEGADA;
        tmp.cant_parametros = 0;
    }

    string_array_destroy(split);
    return tmp;
}

void ejecutar_opcion(t_opcion_consola opcion, char* leido){
    switch(opcion.opcion) {
        case EJECUTAR_SCRIPT:
            printf("Se seleccionó la opción EJECUTAR_SCRIPT\n");
            func_ejecutar_script(leido);
            break;
        case INICIAR_PROCESO:
            printf("Se seleccionó la opción INICIAR_PROCESO\n");
            func_iniciar_proceso(leido);
            break;
        case FINALIZAR_PROCESO:
            printf("Se seleccionó la opción FINALIZAR_PROCESO\n");
            func_finalizar_proceso(leido);
            break;
        case DETENER_PLANIFICACION:
            printf("Se seleccionó la opción DETENER_PLANIFICACION\n");
            func_detener_planificacion();
            break;
        case INICIAR_PLANIFICACION:
            printf("Se seleccionó la opción INICIAR_PLANIFICACION\n");
            func_iniciar_planificacion();
            break;
        case MODIFICAR_MULTIPROGRAMACION:
            printf("Se seleccionó la opción MULTIPROGRAMACION\n");
            func_modificar_multiprogramacion(leido);
            break;
        case PROCESO_ESTADO:
            printf("Se seleccionó la opción PROCESO_ESTADO\n");
            func_proceso_estado();
            break;
        default:
            printf("Opción no válida\n");
            break;
    }
}

void func_ejecutar_script(char* leido){
    //printf("Ejecutando comando func_ejecutar_script\n");

    char** split = NULL;
    split = string_split(leido, " ");

    char* modificado = remover_primer_char_si_machea(split[1], '/'); ////

    FILE* archivo = leer_archivo(modificado);

    free(modificado); ////

    char* buffer = malloc(sizeof(char) * MAX);
    char* linea = NULL; // no es necesario liberar

    while (fgets(buffer, MAX, archivo) != NULL){

        linea = strtok(buffer, "\n");
        procesar_opcion(linea);

        free(buffer);
        buffer = malloc(sizeof(char) * MAX);
    }

    free(buffer);
    cerrar_archivo(archivo);
    string_array_destroy(split);
}

void func_iniciar_proceso(char* leido){
    //printf("Ejecutando comando func_iniciar_proceso\n");
    char** split = string_split(leido, " ");

    sem_wait(&mutex_pid);
    t_PCB* pcb = crear_PCB((uint32_t)contador_pid, (uint32_t)config->quantum, NEW); // creado pero aun no liberado
    contador_pid ++;
    sem_post(&mutex_pid);

    char* modificado = remover_primer_char_si_machea(split[1], '/'); ////

    log_debug(logger, "Avisando a Memoria sobre la entrada de Nuevo Preceso PID: <%u> ", pcb->pid);
    sem_wait(&mutex_conexion_memoria);
        envio_generico_entero_y_string(conexion_memoria, NUEVO_PROCESO_MEMORIA, pcb->pid, modificado);
        validar_respuesta_op_code(conexion_memoria, MEMORIA_OK, logger);
    sem_post(&mutex_conexion_memoria);

    mover_a_new(pcb);    

    free(modificado); ////
    string_array_destroy(split);
}

void func_finalizar_proceso(char* leido){
    //printf("Ejecutando comando func_finalizar_proceso\n");
    char** split = string_split(leido, " ");
    int pid = atoi(split[1]);

    if(finalizar_proceso_NEW(pid)){
        return;
    }

    if(finalizar_proceso_READY(pid)){
        return;
    }
    
    if(algoritmo_elegido == VRR){
        if(finalizar_proceso_READY_AUX(pid)){
            return;
        }
    }

    if(finalizar_proceso_EXECUTE(pid)){ // dijeron que no finalizarian un proceso en ejecucion, capas no hace falta implementarlo
        return;
    }

    if(finalizar_proceso_BLOCKED(pid)){
        return;
    }
    
    if(existe_recursos){
        if(finalizar_proceso_BLOCKED_RECURSO(pid)){
            return;
        }
    }

    string_array_destroy(split);
}

void func_detener_planificacion(void){
    //printf("Ejecutando comando func_detener_planificacion\n");
    // por si quieren detener de nuevo por error
    if(!sistema_detenido){
        sistema_detenido = true;

        sem_wait(&sem_stop_largo_plazo);
        sem_wait(&sem_stop_corto_plazo);
        sem_wait(&sem_stop_cpu_dispatch);
        sem_wait(&sem_stop_io);
    }
}

void func_iniciar_planificacion(void){
    //printf("Ejecutando comando func_iniciar_planificacion\n");
    if(sistema_detenido){
        sistema_detenido = false;

        if(stop_largo_plazo){
            stop_largo_plazo = false;
            sem_post(&sem_stop_largo_plazo);
            sem_post(&sem_stop_largo_plazo);
        }
        else{
            sem_post(&sem_stop_largo_plazo);
        }

        if(stop_corto_plazo){
            stop_corto_plazo = false;
            sem_post(&sem_stop_corto_plazo);
            sem_post(&sem_stop_corto_plazo);
        }
        else{
            sem_post(&sem_stop_corto_plazo);
        }

        if(stop_cpu_dispatch){
            stop_cpu_dispatch = false;
            sem_post(&sem_stop_cpu_dispatch);
            sem_post(&sem_stop_cpu_dispatch);
        }else{
            sem_post(&sem_stop_cpu_dispatch);
        }

        if(stop_io){
            stop_io = false;
            sem_post(&sem_stop_io);
            sem_post(&sem_stop_io);
        }else{
            sem_post(&sem_stop_io);
        }
    }
}

void func_modificar_multiprogramacion(char* leido){
    //printf("Ejecutando comando func_modificar_multiprogramacion\n");
    char** split = string_split(leido, " ");
    int valor = atoi(split[1]);

    printf("grado de multiprogramacion actual: %d \n", grado_multiprogramacion_global);

    int diferencia = valor - grado_multiprogramacion_global;

    if (diferencia > 0) {
        // Aumentar el grado de multiprogramación
        for (int i = 0; i < diferencia; i++) {
            sem_post(&sem_grado_multiprogramacion);
        }
    } else if (diferencia < 0) {
        // Disminuir el grado de multiprogramación
        diferencia *= -1; // Convertir a valor positivo
        for (int i = 0; i < diferencia; i++) {
            sem_wait(&sem_grado_multiprogramacion);
        }
    }

    // Actualizar el valor actual del grado de multiprogramación
    grado_multiprogramacion_global = valor;
    printf("grado de multiprogramacion actualizado: %d \n", grado_multiprogramacion_global);

    string_array_destroy(split);
}

void func_proceso_estado(void){
    //printf("Ejecutando comando func_proceso_estado\n");

    char* lista_new = string_aplanar_PID(cola_new->elements);
    char* lista_ready = string_aplanar_PID(cola_ready->elements);
    
    char* lista_ready_aux = NULL;
    if (algoritmo_elegido == VRR){
        lista_ready_aux = string_aplanar_PID(cola_ready_aux->elements);
    }

    char* lista_execute = string_aplanar_PID(cola_execute->elements);
    char* lista_blocked = string_aplanar_PID(cola_blocked->elements);
    char* lista_exit = string_aplanar_PID(cola_exit->elements);


    printf("Procesos en NEW: %s \n", lista_new);
    printf("Procesos en READY: %s \n", lista_ready);

    if(algoritmo_elegido == VRR){ 
        printf("Procesos en READY AUX: %s \n", lista_ready_aux);
    }

    printf("Procesos en EXECUTE: %s \n", lista_execute);
    printf("Procesos en BLOCKED: %s \n", lista_blocked);
    if(existe_recursos){
        imprimir_recursos_bloqueados();
    }
    printf("Procesos en EXIT: %s \n", lista_exit);


    free(lista_new);
    free(lista_ready);
    if (algoritmo_elegido == VRR){ 
        free(lista_ready_aux);
    }
    free(lista_execute);
    free(lista_blocked);
    free(lista_exit);
}

void imprimir_recursos_bloqueados(void){
    sem_wait(&mutex_diccionario_recursos);
        t_list* lista_recursos = dictionary_elements(recursos);
        t_list* lista_nombre_recursos = dictionary_keys(recursos);
        int tamanio_lista = list_size(lista_recursos);
        char* lista_bloqueados_recurso = NULL;
        char* nombre_recurso = NULL;

        for (int i = 0; i < tamanio_lista; i++){
            t_recurso* recurso = (t_recurso*)list_get(lista_recursos, i);
            nombre_recurso = (char*)list_get(lista_nombre_recursos, i);

            lista_bloqueados_recurso = string_aplanar_PID(recurso->cola_recurso->elements);
            printf("Procesos en BLOCKED RECURSO (%s): %s \n", nombre_recurso, lista_bloqueados_recurso); 
            free(lista_bloqueados_recurso);
            lista_bloqueados_recurso = NULL;     
        }
        
        list_destroy(lista_recursos);
        list_destroy(lista_nombre_recursos);
    sem_post(&mutex_diccionario_recursos);
}

bool finalizar_proceso_NEW(int pid){
    sem_wait(&mutex_cola_new);
    t_PCB* pcb = (t_PCB*)buscar_pcb_por_pid_y_remover(pid, cola_new->elements);
    
    if(pcb == NULL){
        return false;
        sem_post(&mutex_cola_new);
    }

    if(pcb != NULL){
        log_debug(logger, "Finalizando PID: <%u> que estaba en estado <NEW>", pcb->pid);

        char* motivo = obtener_motivo_salida(SALIDA_INTERRUPTED_BY_USER, NULL);
        mandar_a_exit(pcb, motivo);
        free(motivo);
        sem_wait(&sem_procesos_esperando_en_new);

        sem_post(&sem_grado_multiprogramacion);
    }
    sem_post(&mutex_cola_new);

    return true;
}

bool finalizar_proceso_READY(int pid){
    sem_wait(&mutex_cola_ready);
    t_PCB* pcb = (t_PCB*)buscar_pcb_por_pid_y_remover(pid, cola_ready->elements);

    if(pcb == NULL){
        sem_post(&mutex_cola_ready);
        return false;
    }

    if(pcb != NULL){
        log_debug(logger, "Finalizando PID: <%u> que estaba en estado <READY>", pcb->pid);

        char* motivo = obtener_motivo_salida(SALIDA_INTERRUPTED_BY_USER, NULL);
        mandar_a_exit(pcb, motivo);
        free(motivo);
        sem_wait(&sem_procesos_esperando_en_ready);

        sem_post(&sem_grado_multiprogramacion);
    }
    sem_post(&mutex_cola_ready);

    return true;
}

bool finalizar_proceso_READY_AUX(int pid){
    sem_wait(&mutex_cola_ready_aux);
        t_PCB* pcb = (t_PCB*)buscar_pcb_por_pid_y_remover(pid, cola_ready_aux->elements);

    if(pcb == NULL){
        sem_post(&mutex_cola_ready_aux);
        return false;
    }

    if(pcb != NULL){
        log_debug(logger, "Finalizando PID: <%u> que estaba en estado <READY_AUX>", pcb->pid);

        char* motivo = obtener_motivo_salida(SALIDA_INTERRUPTED_BY_USER, NULL);
        mandar_a_exit(pcb, motivo);
        free(motivo);
        sem_wait(&sem_procesos_esperando_en_ready);

        sem_post(&sem_grado_multiprogramacion);
    }
    sem_post(&mutex_cola_ready_aux);

    return true;
}

bool finalizar_proceso_BLOCKED_RECURSO(int pid){
    t_PCB* pcb = NULL;

    sem_wait(&mutex_diccionario_recursos);

    t_list* lista_recursos = dictionary_elements(recursos);
    int tamanio_lista = list_size(lista_recursos);

    for (int i = 0; i < tamanio_lista; i++){
        t_recurso* recurso = (t_recurso*)list_get(lista_recursos, i);

        pcb = (t_PCB*)buscar_pcb_por_pid_y_remover(pid, recurso->cola_recurso->elements);
        if(pcb != NULL){
            break;
        }
    }

    list_destroy(lista_recursos);
        
    sem_post(&mutex_diccionario_recursos);

    if(pcb == NULL){
        return false;
    }

    if(pcb != NULL){
        log_debug(logger, "Finalizando PID: <%u> que estaba en estado <BLOCKED_RECURSO>", pcb->pid);
        
        char* motivo = obtener_motivo_salida(SALIDA_INTERRUPTED_BY_USER, NULL);
        mandar_a_exit(pcb, motivo);
        free(motivo);

        sem_post(&sem_grado_multiprogramacion);
    }

    return true;
}

bool finalizar_proceso_EXECUTE(int pid){
    /*  
        Habria que considerar 2 casos, en donde el pcb esta excutanto en cpu y aun no
        volvio a kernel, en donde ya esta en kernel y esta en medio de una funcion o caso
        por ejemplo WAIT, SIGNAL, DESALOJO, PETICION IO, ya que puede estar en medio de 
        operaciones o por hacer un transicion.
        - para el primer caso ponemos el pid en una lista y asi cuando llegue el proceso
          a kernel si esta en la lista elimina el proceso y hace un post(cpu_libre), luego ver
          si podemos eliminar de antemano el pcb viejo y luego el pcb nuevo.
        - para el segundo caso podriamos sacar el proceso viejo de la cola, y antes de cada
          trnasicion preguntar si la cola execute esta vacia, en ese caso terminar de ejecutar
          la funcion que se esta ejecutando.
        PUEDE haber un tercer caso en donde por alguna motivo ya se paso ambas funciones
        y justo esta dentro de una funcion que hace un transicion, en ese caso ya no tenemos
        control ya que la cola execute estaria vacia (porque dentro de ls funciones trnasicion 
        se saca de la cola al proceso viejo), en este caso se pondria en otro estado y no seria
        nuestro problema.
        Recordemos que con el Mutex solo pueden estar de a uno en la zona critica.
    */
    t_PCB* pcb = NULL;

    sem_wait(&mutex_cola_execute);
    pcb = buscar_pcb_por_pid_y_obtener(pid, cola_execute->elements);

    if(pcb == NULL){
        sem_post(&mutex_cola_execute);
        return false;
    }

    if(pcb != NULL){
        sem_wait(&mutex_proceso_en_ejecucion);
        if(proceso_en_ejecucion){
            // luego que llegue el proceso nos encargamos de dejar la cpu libre y mandar a exit en el case.
            log_debug(logger, "PID <%u> pendiente de Finalizacion cuando vuelva de CPU", pcb->pid);
            
            // con esto treamos el proceso de CPU en caso que no este
            sem_wait(&mutex_conexion_cpu_interrupt);
                envio_generico_op_code(conexion_cpu_interrupt, DESALOJO);
            sem_post(&mutex_conexion_cpu_interrupt);

            finalizacion_execute_afuera_kernel = true;
        }
        else{
            // antes de cada transicion hacer un if y mandarlo a exit | capas usamos una varibale global
            log_debug(logger, "PID <%u> pendiente de Finalizacion ante una transicion", pcb->pid);
            finalizacion_execute_dentro_kernel = true;
        }
        sem_post(&mutex_proceso_en_ejecucion);
    }
    
    sem_post(&mutex_cola_execute);

    return true;   
}

bool finalizar_proceso_BLOCKED(int pid){
    /*
    si esta en blocked, es porque esta por realizar o esperando un IO
    - si esta por realizarlo, significa que t_pendiente de io con su pid
      esta en cola. (ya que cuando se manda a relaizar un io el t_pendiente
      se elimina). eliminamos el t_pendiente de io de esa interfaz y removemos 
      proceso de bloqueados.
    - si ya esta realizando un io y esta esperando a que termine, agreamos su pid
      a una lista global de victimas io, para que cuando llegue el proceso a kernel
      se fije si esta dentro de la lista el pid, asi lo saca de bloquedos.
    -> Una vez removido analizar que semaforos se hacen post y cuales wait.
    -> envolver todo en el semaforo de diccionario recursos para evitar inconsistencias
    */
    
    t_PCB* pcb = NULL;
    t_interfaz* interfaz = NULL;
    t_io_pendiente* io_pendiente = NULL;

    sem_wait(&mutex_cola_blocked);
    pcb = buscar_pcb_por_pid_y_obtener(pid, cola_blocked->elements);

    if(pcb == NULL){
        sem_post(&mutex_cola_blocked);
        return false;
    }

    if(pcb != NULL){
        pcb = NULL; // libero para poder usarlo despues

        sem_wait(&mutex_diccionario_interfaces);
        t_list* lista_interfaces = dictionary_elements(interfaces);
        int tamanio_lista = list_size(lista_interfaces);

        for (int i = 0; i < tamanio_lista; i++){
            interfaz = (t_interfaz*)list_get(lista_interfaces, i);

            io_pendiente = buscar_io_pendiente_por_pid_y_remover((uint32_t)pid, interfaz->cola->elements); 
            
            if(io_pendiente != NULL){
                // si entramos aca es porque aun no realizo su io
                pcb = buscar_pcb_por_pid_y_remover(pid, cola_blocked->elements);
                
                log_debug(logger, "Finalizando PID: <%u> que estaba en estado <BLOCKED>", pcb->pid);

                char* motivo = obtener_motivo_salida(SALIDA_INTERRUPTED_BY_USER, NULL);
                mandar_a_exit(pcb, motivo);
                free(motivo);
                sem_wait(&sem_peticiones_io_por_procesar);

                sem_post(&sem_grado_multiprogramacion);

                liberar_elemento_t_io_pendiente((void*) io_pendiente);

                list_destroy(lista_interfaces);
                sem_post(&mutex_diccionario_interfaces);
                sem_post(&mutex_cola_blocked); 
                return true; // salimos de la funcion
            }
        }

        if (io_pendiente == NULL){
            // si entramos aca es porque termino el ciclo pero nunca encontro un io.
            // por lo que seguro esta en t_list* io_pendientes_ejecutando;
            // agregamos el pid la lista global de victimas de io.
            // pasamos la responsbilidad al hilo procesar_io;
            t_io_victima* victima = crear_t_io_victima((uint32_t)pid, USUARIO_CONSOLA, NULL);
            sem_wait(&mutex_victimas_pendientes_io);
                list_add(victimas_pendientes_io, (void*) victima);
            sem_post(&mutex_victimas_pendientes_io);    
            log_debug(logger, "Finalizacion PID: <%d> pendiente por estar ejecutando IO", pid);
        }

        list_destroy(lista_interfaces);
        sem_post(&mutex_diccionario_interfaces);
    }

    sem_post(&mutex_cola_blocked);

    return true;
}

void finalizar_proceso_EXIT(int pid){
    /*
    esta funcion no deberia existir, pero por el momento nosotros los precesos
    en exit no los liberamos, podriamos antes de liberalos imprimir sus estadisticas
    en solo un string, y luego liberarlo. esto hacer si sibra tiempo. 
    si no liberar el pcb y solo guardar su pid (puntero entero con memoria asignada)
    como para tener un registro de los pid que finalizaron sin necesidad de tener memoria 
    acumulada.
    */
}

bool pendiente_de_finalizacion_fuera_de_kernel(t_PCB* pcb_nueva){
    sem_wait(&mutex_cola_execute);
    if(finalizacion_execute_afuera_kernel){
        finalizacion_execute_afuera_kernel = false;

        t_PCB* tmp = (t_PCB*) queue_pop(cola_execute);
        
        // liberamos el pcb viejo | actualizar contexto
        liberar_PCB(tmp);
        
        // mandamos el pcb recien llegado, a exit (estara actualizado)
        char* motivo = obtener_motivo_salida(SALIDA_INTERRUPTED_BY_USER, NULL);
        mandar_a_exit(pcb_nueva, motivo);
        free(motivo);

        sem_post(&mutex_cola_execute);
        return true;
    }
    else{
        sem_post(&mutex_cola_execute);
        return false;
    }
}