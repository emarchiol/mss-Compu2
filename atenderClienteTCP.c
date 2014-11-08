/*
        //Le cuento al cliente que está todo bien y le digo que le voy a transmitir
        char cabecera[256];
        leido = snprintf (cabecera, sizeof cabecera, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 19\n\n"); //Transfer-Encoding: chunked\nConnection: keep-alive\n
        
        if(write (client_sd, cabecera, leido)<0)
            perror("fracaso al escribir la cabecera");
*/
#include "funciones.h"

void atenderClienteTCP(int *socketD){
printf("\n Hilo TCP inicializado\n");
	int sd = *socketD;
	char buf[1024];
    char lastPacket[1024];
	int tamBuf = sizeof buf;
	int leido;
	int fd;


	memset(buf,0,tamBuf);
    //Todo esto hasta el while es para el archivo log que no está implementado
	if ((fd = open ("clienteTCP.log", O_WRONLY | O_CREAT)) < 0)
        {
            perror("fracaso en abrir el archivo log, open dijo:");
            pthread_exit (NULL);
        }
    write(fd,"\n\n==== NEW CLIENT ====\n\n",24);
    memset(buf,0,tamBuf);
    memset(lastPacket,0,tamBuf);

    while((leido = read(sd, buf, tamBuf))>0){
                //Log file, este write es para escribir en el archivo de log
                //write(fd,buf,tamBuf);

                //El cliente dijo:
                printf("%s",buf);
                /*Se fija si en el mensaje que me manda el cliente los primero caracteres
                corresponden a OPTIONS, si es así, le responder con 200 OK y las opciones,
                se supone que el cliente debería seleccionar una opción, pero no hace nada*/

                if( (memcmp("OPTIONS",buf,7)!=0) && (memcmp("OPTIONS",lastPacket,7)==0) ) {
                    printf("\n-- Enviando respuesta... --\n");
                    memset (buf, 0, tamBuf);
                    //Respuesta
                    //Nota que el CSeq debe ser igual al del cliente
                    memcpy(buf,"RTSP/1.0 200 OK\nCSeq: 1\nPublic: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n",74);
                    //printf("RTSP/1.0 200 OK\nCSeq: 2\nPublic: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\n");
                    write(sd,buf,sizeof buf);
                }
            memcpy(lastPacket,buf,sizeof buf);
            memset (buf, 0, tamBuf);
    }
    if(leido==-1)
        perror("Fracaso en el read:");
        //Cierro la conexión con el cliente
        printf("Cerrando hilo TCP...");
        close (sd);
		pthread_exit (NULL);
}