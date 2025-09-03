#include "client.h"


int main(void){
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();
	if(logger == NULL){
		printf("Error al iniciar el logger\n");
		abort();
	}

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"
	log_info(logger, "Hola! Soy un log");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();
	if(config == NULL){
		printf("Error al iniciar el config\n");
		abort();
	}

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "CLAVE");

	// Loggeamos el valor de config
	log_info(logger, "IP: %s", ip);
	log_info(logger, "PUERTO: %s", puerto);
	log_info(logger, "CLAVE: %s", valor);

	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje(valor, conexion);

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);
	printf("Termino el programa\n");

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void){
	t_log* nuevo_logger;
	nuevo_logger = log_create("tp0.log", "TP0", true, LOG_LEVEL_INFO);

	return nuevo_logger;
}

t_config* iniciar_config(void){
	t_config* nuevo_config;
	nuevo_config = config_create("cliente.config");

	return nuevo_config;
}

void leer_consola(t_log* logger){
	char* leido;

	printf("\nIngrese un texto (escriba 'exit' para salir):\n");
	while(1){
		leido = readline(">");
        if (!leido || strcmp(leido, "exit") == 0) { //(ctrl + d) = null
            free(leido);
			break;
        }
		add_history(leido);
        log_info(logger, "%s\n", leido);
        free(leido);
    }
}

void paquete(int conexion){
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete;

	// Leemos y esta vez agregamos las lineas al paquete
	paquete = crear_paquete();
	while(1){
		leido = readline(">");
		if(!leido || strcmp(leido, "exit") == 0){
			free(leido);
			break;
		}
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		free(leido);
	}
	enviar_paquete(paquete, conexion);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log* logger, t_config* config) {
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
}
