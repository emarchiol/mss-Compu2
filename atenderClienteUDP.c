//================================================================================================
/* Abre un archivo y lo envía por el socket, lo dejo acá en caso de que sirve después
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
        //Cierro archivo
        close (fd);
*/
//================================================================================================
#include "funciones.h"

void atenderClienteUDP(int *socketD, client_packet packetRTSP){
printf("\nCliente UDP inicializado, no hago nada");
    //Tiene que crear un nuevo socket con otro puerto previamente establecido mediante SETUP y desde server.c se tiene que pasar la IP del cliente
/*
	int sd = *socketD;
	char buf[1024];
	int tamBuf = sizeof buf;
	int leido;
	int fd;
	memset(buf,0,tamBuf);

    int sd_UDP;
    sd_UDP = socket(AF_INET,SOCK_DGRAM,0);
    bind(sd_UDP, (struct sockaddr *) &sdCliente,sizeof(sdCliente));


	while ((leido = read(sd, buf, tamBuf)) > 0)
        {
            write(fd,buf,tamBuf);
            memset (buf, 0, tamBuf);
        }

        close (sd);
		pthread_exit (NULL);*/
}