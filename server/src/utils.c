#include"utils.h"

t_log* logger;

int iniciar_servidor(void){
	int socket_servidor;
	int err;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	err = getaddrinfo(NULL, PUERTO, &hints, &servinfo);
	if (err != 0) {
		show_error("getaddrinfo fallo");
		log_error(logger, "getaddrinfo: %s", gai_strerror(err));
		abort();
	}

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	
	// Asociamos el socket a un puerto
	err = setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));
	if (err != 0) {
		show_error("setsockopt fallo");
		log_error(logger, "setsockopt: %s", strerror(errno));
		abort();
	}
	err = bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	if (err != 0) {
		show_error("bind fallo");
		log_error(logger, "bind: %s", strerror(errno));
		abort();
	}

	// Escuchamos las conexiones entrantes
	err = listen(socket_servidor, SOMAXCONN);
	if (err != 0) {
		show_error("listen fallo");
		log_error(logger, "listen: %s", strerror(errno));
		abort();
	}

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor){
	// Aceptamos un nuevo cliente
	int socket_cliente;

	socket_cliente = accept(socket_servidor, NULL, NULL);
	if(socket_cliente == -1){
		log_error(logger, "Error al aceptar cliente: %s", strerror(errno));
		return -1;
	}

	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
