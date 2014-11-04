/*
        //Le cuento al cliente que está todo bien y le digo que le voy a transmitir
        char cabecera[256];
        leido = snprintf (cabecera, sizeof cabecera, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 19\n\n"); //Transfer-Encoding: chunked\nConnection: keep-alive\n
        
        if(write (client_sd, cabecera, leido)<0)
            perror("fracaso al escribir la cabecera");
*/
#include "funciones.h"

void atenderClienteUDP(int *socketD){
printf("\nCliente UDP inicializado");
	int sd = *socketD;
	char buf[1024];/*
	int tamBuf = sizeof buf;
	int leido;
	int fd;
	memset(buf,0,tamBuf);

	if ((fd = open ("clienteUDP.log", O_WRONLY | O_CREAT)) < 0)
        {
            perror("fracaso en abrir el archivo log, open dijo:");
            pthread_exit (NULL);
        }
    write(fd,"\n\n==== NEW CLIENT ====\n\n",24);
    memset(buf,0,tamBuf);


	while ((leido = read(sd, buf, tamBuf)) > 0)
        {
            write(fd,buf,tamBuf);
            memset (buf, 0, tamBuf);
        }
*/
        close (sd);
		pthread_exit (NULL);
}