#include "funciones.h"

//================================================================================================================================================
    /*  
    Computación II - 2014 - Universidad de Mendoza
    sMM version 0.1a

    Espera una petición con accept, una vez que haya una nueva petición inicia el hilo TCP para la comunicación RTSP, analiza/parsea los mensajes
    del cliente con analizarRespuestaRTSP.c y responde adecuadamente mediante construirRespuestaRTSP.c. Si el cliente solicita PLAY, entonces 
    atenderClienteTCP.c disparará a atenderClienteUDP.c para empezar el streaming de video especificado por DESCRIBE y habiendo configurado la 
    conexion previamente por SETUP.
    Además server.c setea la key de comunicación de los hilos, de esta manera las claves serán unicas y no debería haber problemas de comunicación
    con múltiples clientes conectados. Se utiliza un semaforo para que el hilo TCP pueda tomar los datos que le pasa server.c sin que estos sean
    sobreescritos antes por otro "accept()".
    */
//================================================================================================================================================
void killServer(int signum);



void killServer(int signum){
    write(STDOUT_FILENO, "\n-- Shutting down server by user... --\n", 39);   
    sem_close(semaforo);
    sem_unlink("/semaforo"); 
    exit(signum);
}

int main()
{

    int port = 554;
    //Estructura que lleva informacion de conexion como puerto y demás relacionada con el socket de conexion
    //sdCliente vendría a ser la estructura de mi socket y structClient la estructura del cliente al cual le acepto la conexion
    struct sockaddr_in sdCliente;
    struct sockaddr_in structClient;
    memset(&sdCliente,0,sizeof sdCliente);
    memset(&structClient,0,sizeof structClient);

    //Descriptor del socket
    int sock_descriptor;

    //Descriptor temporal para administrar las peticiones del cliente
    int client_sd;
    socklen_t address_size;
    pthread_t rid;

    //Capturo señal CTRL+C para liberar el puerto apropiadamente
    signal(SIGINT, killServer);

    //Generador de key para las colas 
    srand(time(NULL));
    int r = rand();
    char keyQ[8];
    memset(keyQ, 0, 8);
    //Estructura con info del cliente
    tcp_info clientInfo;

    //=========================
    //=========================

    //Semaforo
    if ((semaforo = sem_open ("/semaforo", O_CREAT | O_RDWR, 0666, 0)) < 0)
        perror ("server->Ups, sem_open dijo");

    //Creo el socket y lo asocio al descriptor
    sock_descriptor =  socket(AF_INET, SOCK_STREAM,0);

    //Si no pude abrir el socket, error y chau
    if(sock_descriptor == -1){
    	perror("server->Ups, llamada a socket dijo");
    	exit(1);
    }

    sdCliente.sin_family = AF_INET;
    sdCliente.sin_addr.s_addr = INADDR_ANY;
    //htons convierte el puerto "int" a puerto de "red"
    sdCliente.sin_port = htons(port);

    //Todo listo, intento bindear la estructura al socket
    if(bind(sock_descriptor,(struct sockaddr *)&sdCliente, sizeof(sdCliente))== -1) {
    	perror("server->Ups, bind dijo");
    	exit(1);
    }

    //Ahora puedo escuchar conexiones entrantes, OJO con el tercer parametro que identifica la cantidad
    //de conexiones que pueden quedar en cola
    if (listen(sock_descriptor,1)==-1){
    	perror("server->Ups, listen dijo");
    	exit(1);
    }

    //=========================
        //Inicio servidor
    //=========================
    write(STDOUT_FILENO, "\nEsperando conexiones...", 24);

    //Inicio servidor
    address_size = sizeof(struct sockaddr);

    while((client_sd = accept(sock_descriptor, (struct sockaddr *)&structClient, &address_size)) >0 ){
        
        //Acepto la conexion
        write(STDOUT_FILENO, "\n\n=============================", 31);
        write(STDOUT_FILENO, "\n-- Nuevo cliente conectado --\n", 31);
        write(STDOUT_FILENO, "=============================\n", 30);

        clientInfo.sd = client_sd;
        //Parseo el ip del cliente
        sprintf(clientInfo.ip,"%d.%d.%d.%d\n",(int)(structClient.sin_addr.s_addr&0xFF),(int)((structClient.sin_addr.s_addr&0xFF00)>>8),(int)((structClient.sin_addr.s_addr&0xFF0000)>>16),(int)((structClient.sin_addr.s_addr&0xFF000000)>>24));

        //Genero la key unica para la cola de TCP y UDP
        snprintf(keyQ, 8, "%d", r);
        memcpy(keyQ, "/", 1);
        strncpy(clientInfo.keyQ, keyQ, 8); //Asigno key de cola
        r = rand();
        memset(keyQ, 0, 8);

        //Disparo el hilo TCP
        if(pthread_create(&rid, NULL, (void*)atenderClienteTCP, (void*)&clientInfo)<0)
            perror("server->Ups, pthread_create dijo");

        //Liberación de recursos, tengo que esperar a que el hilo copie los recursos de clientInfo antes de liberar los recursos y poder aceptar otro cliente
        if(sem_wait(semaforo)<0)
            perror("server->Ups, sem_wait dijo");
        //Reseteo la estructura
        clientInfo.sd = 0;
        memset(clientInfo.keyQ, 0, sizeof(clientInfo.keyQ));
        memset(clientInfo.ip, 0, sizeof(clientInfo.ip));
    }
    if(close(sock_descriptor)<0)
        perror("server->Ups, close dijo");
    return 0;
}
