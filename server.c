#include "funciones.h"

/*Lo que hace el pgma de momento:
Espera una petición con accept, una vez que haya una nueva petición inicia un hilo
para la comunicación RTSP en TCP (hilo atenderClienteTCP).
El chorizo que está comentando es el envío de un archivo por socket.
*/

int main()
{



    int port = 8000;
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
    char buf[1048576]; //1mb de buffer
    int tamBuf;
    tamBuf = sizeof buf;
    memset(buf,0,tamBuf);

    pthread_t rid;


    //=========================
    //=========================

    //Creo el socket y lo asocio al descriptor
    sock_descriptor =  socket(AF_INET, SOCK_STREAM,0);

    //Si no pude abrir el socket, error y chau
    if(sock_descriptor == -1){
    	perror("llamada a socket");
    	exit(1);
    }

    sdCliente.sin_family = AF_INET;
    sdCliente.sin_addr.s_addr = INADDR_ANY;
    //htons convierte el puerto "int" a puerto de "red"
    sdCliente.sin_port = htons(port);

    //Todo listo, intento bindear la estructura al socket
    if(bind(sock_descriptor,(struct sockaddr *)&sdCliente, sizeof(sdCliente))== -1) {
    	perror("fracaso en el bind");
    	exit(1);
    }

    //Ahora puedo escuchar conexiones entrantes, OJO con el tercer parametro que identifica la cantidad
    //de conexiones que pueden quedar en cola
    if (listen(sock_descriptor,1)==-1){
    	perror("fracaso la escucha");
    	exit(1);
    }

    write(STDOUT_FILENO, "\nEsperando conexiones...", 24);

    //Inicio servidor
    address_size = sizeof(struct sockaddr);
    while((client_sd = accept(sock_descriptor, (struct sockaddr *)&structClient, &address_size)) >0 ){
    	//Acepto la conexion
        write(STDOUT_FILENO, "\n-- Nuevo cliente conectado --\n", 31);
        //Ip del cliente
        tcp_info clientInfo;
        clientInfo.sd = client_sd;
        sprintf(clientInfo.ip,"%d.%d.%d.%d\n",(int)(structClient.sin_addr.s_addr&0xFF),(int)((structClient.sin_addr.s_addr&0xFF00)>>8),(int)((structClient.sin_addr.s_addr&0xFF0000)>>16),(int)((structClient.sin_addr.s_addr&0xFF000000)>>24));
        //Hilo de lectura TCP FALTA PASAR LA IP DEL CLIENTE
        pthread_create(&rid, NULL, (void*)atenderClienteTCP, (void*)&clientInfo);
    }
    close (client_sd);    
    close(sock_descriptor);
    return 0;
}