#include "funciones.h"

/*La idea principal sería:
1) Iniciar servidor y esperar peticiones
2) Una vez aceptada la petición crear un hilo para esa petición
3) leer que nos solicita el cliente
4) entregarle lo que el cliente desea
5) cerrar el hilo
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
    int sd_UDP;
    //Descriptor temporal para administrar las peticiones del cliente
    int client_sd;
    socklen_t address_size;
    char buf[1048576]; //1mb de buffer
    int tamBuf;
    tamBuf = sizeof buf;
    memset(buf,0,tamBuf);
    int leido;
    int fd;
    //hilo lectura
    pthread_t rid;

    //=========================
    //=========================

    //Creo el socket y lo asocio al descriptor
    sock_descriptor =  socket(AF_INET, SOCK_STREAM,0);
    sd_UDP = socket(AF_INET,SOCK_DGRAM,0);
    //Si no pude abrir el socket, error y chau
    if(sock_descriptor == -1){
    	perror("llamada a socket");
    	exit(1);
    }

    sdCliente.sin_family = AF_INET;
    sdCliente.sin_addr.s_addr = INADDR_ANY;
    //htons convierte el puerto "int" a puerto de "red" o algo así
    sdCliente.sin_port = htons(port);

    //Todo listo, intento bindear la estructura al socket
    if(bind(sock_descriptor,(struct sockaddr *)&sdCliente, sizeof(sdCliente))== -1) {
    	perror("fracaso en el bind");
    	exit(1);
    }
    bind(sd_UDP, (struct sockaddr *) &sdCliente,sizeof(sdCliente));

    //Ahora puedo escuchar conexiones entrantes, OJO con el tercer parametro que identifica la cantidad
    //de conexiones que pueden quedar en cola
    if (listen(sock_descriptor,1)==-1){
    	perror("fracaso la escucha");
    	exit(1);
    }

    printf("Ahora si, aceptamos conexiones!\n");

    //Inicio servidor
    address_size = sizeof(struct sockaddr);
    while((client_sd = accept(sock_descriptor, (struct sockaddr *)&structClient, &address_size)) >0 ){
    	//Acepto la conexion
    	printf("Nueva conexion establecida !\n");

        //Leo desde el protocolo TCP
        pthread_create(&rid, NULL, (void*)atenderClienteTCP, (void*)&client_sd);
        //Leo desde el protocolo UDP
        pthread_create(&rid, NULL, (void*)atenderClienteUDP, (void*)&client_sd);

        //Abro y le paso el archivo
        if ((fd = open ("sample.mp4", O_RDONLY)) < 0)
        {
            perror("fracaso en abrir el archivo, open dijo:");
            return -1;
        }

        memset(buf, 0, tamBuf);
        while ((leido = read(fd, buf, tamBuf)) > 0)
        {
            printf("Bytes leidos del archivo: %d\n",leido);
            leido = write (client_sd, buf, leido);
            printf("Bytes enviados: %d\n",leido);
            memset (buf, 0, tamBuf);
        }

        //Cierro archivo y conexion
        close (fd);    
        close (client_sd);
        printf("Conexion con el cliente terminada\n");
    }
    
    close(sock_descriptor);
    return 0;
}