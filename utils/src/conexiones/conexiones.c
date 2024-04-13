#include <conexiones/conexiones.h>

int crear_conexion(char* ip, char* puerto, char* nombreDelProceso, t_log* logger){
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,
                    	        server_info->ai_socktype,
                                server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == 0)
        log_info(logger, "Conectado con el Modulo: %s ", nombreDelProceso);
	else
        log_error(logger, "No se pudo conectar con el Modulo: %s", nombreDelProceso);
		//log_error(logger, "Finalizando Modulo:");
		//exit(EXIT_FAILURE);
	
	// liberar memoria
	freeaddrinfo(server_info);

	return socket_cliente;
}

int iniciar_servidor(char* name ,char* ip, char* puerto, t_log* logger){

	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	
	getaddrinfo(ip, puerto, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family,
                         	 servinfo->ai_socktype,
                         	 servinfo->ai_protocol);

	// Asociamos el socket a un puerto (toma le socket recien creado y lo pega al puerto)
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes (toma el socker y lo marca en el sistema cuya responsabilidad es notificar cuando un nuevo cliente esta intentando conectarse)
	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);

	log_info(logger, "Servidor: %s Levantado en IP:%s Puerto:%s",name, ip, puerto);

	return socket_servidor;
}


int server_listen(t_log* logger, char* server_name, int server_socket, void(*func_procesar_conexion)(void*)) {
    int cliente_socket = esperar_clientes(logger, server_name, server_socket);

    if (cliente_socket != -1) {
        pthread_t hilo;
        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
        args->log = logger;
        args->fd_cliente = cliente_socket;
        args->server_name = server_name;

        pthread_create(&hilo, NULL, (void*)func_procesar_conexion, (void*) args);
        pthread_detach(hilo);
        return 1;
    }
    
    return 0;
}


int esperar_clientes(t_log* logger, const char* name, int socket_servidor) {
    struct sockaddr_in dir_cliente;
    socklen_t tam_direccion = sizeof(struct sockaddr_in);

    int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
	//printf("Cliente conectado (a %s)\n", name);
    log_info(logger, "Cliente conectado (a %s)", name);

    return socket_cliente;
}

void procesar_conexion_template(void *args){
    t_procesar_conexion_args *casted_args = (t_procesar_conexion_args *)args;
    t_log* logger = casted_args->log; 
    int socket = casted_args->fd_cliente;
    char *nombre_servidor = casted_args->server_name;
    free(casted_args);

    while (1) {
        int cod_op = recibir_operacion(socket);
        log_info(logger, "Se recibió el cod operacion %d  - %s", cod_op, nombre_servidor);
        switch (cod_op) {
            case 1:
                // Hacer algo si el código de operación es 1 (usar ENUM)
                break;
            case -1:
                log_error(logger, "Cliente desconectado de %s", nombre_servidor);
                //return EXIT_FAILURE -- si queremos terminar el server apenas alguien se desconecte
                break;
            default:
                log_error(logger, "El codigo de operacion %d es incorrecto - %s", cod_op, nombre_servidor);
                break;
        }
    }
}

int recibir_operacion(int socket_cliente){
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0){
		return cod_op;}
	else{
		close(socket_cliente);
		return -1;
	}
}

void liberar_conexion(int socket_cliente){
	close(socket_cliente);
}
