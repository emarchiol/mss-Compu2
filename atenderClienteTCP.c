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

    while((leido = read(sd, buf, tamBuf))>0){
                //Log file, este write es para escribir en el archivo de log
                //write(fd,buf,tamBuf);

                //El cliente dijo:
                write(STDOUT_FILENO, buf, sizeof buf);
                /*Se fija si en el mensaje que me manda el cliente los primero caracteres
                corresponden a OPTIONS, si es así, le responder con 200 OK y las opciones,
                se supone que el cliente debería seleccionar una opción, pero no hace nada*/

                if( (memcmp("OPTIONS",buf,7)==0) ) {
                    sleep(5);
                    write(STDOUT_FILENO, "\n-- Enviando respuesta... --\n", 29);
                    memset (buf, 0, tamBuf);
                    //Respuesta
                    //Nota que el CSeq debe ser igual al del cliente
                    memcpy(buf,"RTSP/1.0 200 OK\r\nCSeq: 1\r\nPublic: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n",76);
                    //printf("RTSP/1.0 200 OK\nCSeq: 2\nPublic: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\n");
                    write(sd, buf, 76);
                }

                if( (memcmp("DESCRIBE",buf,8)==0) ) {
                    sleep(2);
                    write(STDOUT_FILENO, "\n-- Enviando respuesta2... --\n", 30);
                    memset (buf, 0, tamBuf);
                    //memcpy(buf, "RTSP/1.0 501 Not Implemented\r\nCSeq: 2\r\n\r\n", 41);
                    //Respuesta
                    //Nota que el CSeq debe ser igual al del cliente
                    memcpy(buf,"RTSP/1.0 200 OK\r\nCSeq: 2\r\nContent-Base: rtsp://localhost:8000/\r\nContent-Type: application/sdp\r\nContent-Length: 462\r\n\r\nm=video 0 RTP/AVP 96\r\na=control:streamid=0\r\na=range:npt=0-7.741000\r\na=length:npt=7.741000\r\na=rtpmap:96 MP4V-ES/5544\r\na=mimetype:string;'video/MP4V-ES'\r\na=AvgBitRate:integer;304018\r\na=StreamName:string;'hinted video track'\r\nm=audio 0 RTP/AVP 97\r\na=control:streamid=1\r\na=range:npt=0-7.712000\r\na=length:npt=7.712000\r\na=rtpmap:97 mpeg4-generic/32000/2\r\na=mimetype:string;'audio/mpeg4-generic'\r\na=AvgBitRate:integer;65790\r\na=StreamName:string;'hinted audio track'\r\n\r\n", 580);
                    //printf("RTSP/1.0 200 OK\nCSeq: 2\nPublic: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\n");
                    write(sd, buf, 580);
                }


            memset (buf, 0, tamBuf);
    }
    if(leido==-1)
        perror("Fracaso en el read:");
        //Cierro la conexión con el cliente
        printf("Cerrando hilo TCP...");
        close (sd);
		pthread_exit (NULL);
}
