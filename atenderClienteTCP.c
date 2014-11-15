
//========================================================================
    /*
    Este hilo se dispara cuando un cliente se conecta al servidor por
    server.c, se encarga de leer lo que el cliente envía y de responder 
    según corresponda solo si el mensaje es completo en su formato, de otro
    modo sigue leyendo del socket.

    buf es el contenido de la lectura que envió el cliente hasta un doble
    CRLF, lecturaCompleta se va llenando con buf representando así un
    paquete completo.
    */
//========================================================================


#include "funciones.h"

void atenderClienteTCP(int *socketD){
    
	int sd = *socketD;
	char buf[1024];
    char lecturaCompleta[2048];
    char * plecturaCompleta = lecturaCompleta;
	int tamBuf = sizeof buf;
	int leido;
    client_packet respuestaRTSP;


	memset(buf, 0, tamBuf);
    memset(lecturaCompleta, 0, tamBuf);
//====================================
    /*Empieza lectura del socket*/
//====================================

    write(STDOUT_FILENO,"\n Hilo TCP inicializado\n",24);
    while((leido = read(sd, buf, tamBuf))>0){


        respuestaRTSP = analizarRespuestaRTSP(buf, plecturaCompleta);

        //Si el mensaje es correcto le respondo sino seguiré leyendo
        if(respuestaRTSP.pckComplete==true){

            write(STDOUT_FILENO, "\n  -Client Said:\n", 17);
            write(STDOUT_FILENO, lecturaCompleta, strlen(lecturaCompleta));

            if(memcmp(respuestaRTSP.method, "OPTIONS", strlen(respuestaRTSP.method))==0)
            {
                write(STDOUT_FILENO, "\n-- Enviando respuesta OPTIONS... --\n", 38);
                memset (buf, 0, tamBuf);
                //Respuesta
                //Nota que el CSeq debe ser igual al del cliente
                memcpy(buf,"RTSP/1.0 200 OK\r\nCSeq: 1\r\nPublic: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n",76);
                
                write(sd, buf, 76);
            }
            else if(memcmp(respuestaRTSP.method,"DESCRIBE",strlen(respuestaRTSP.method))==0)
            {
                write(STDOUT_FILENO, "\n-- Enviando respuesta DESCRIBE... --\n", 39);
                memset (buf, 0, tamBuf);
                //Respuesta
                memcpy(buf,"RTSP/1.0 200 OK\r\nCSeq: 2\r\nContent-Base: rtsp://localhost:8000/\r\nContent-Type: application/sdp\r\nContent-Length: 462\r\n\r\nm=video 0 RTP/AVP 96\r\na=control:streamid=0\r\na=range:npt=0-7.741000\r\na=length:npt=7.741000\r\na=rtpmap:96 MP4V-ES/5544\r\na=mimetype:string;'video/MP4V-ES'\r\na=AvgBitRate:integer;304018\r\na=StreamName:string;'hinted video track'\r\nm=audio 0 RTP/AVP 97\r\na=control:streamid=1\r\na=range:npt=0-7.712000\r\na=length:npt=7.712000\r\na=rtpmap:97 mpeg4-generic/32000/2\r\na=mimetype:string;'audio/mpeg4-generic'\r\na=AvgBitRate:integer;65790\r\na=StreamName:string;'hinted audio track'\r\n\r\n", 580);
                write(sd, buf, 580);
            }
            else if(memcmp(respuestaRTSP.method,"SETUP",strlen(respuestaRTSP.method))==0){
                write(STDOUT_FILENO, "\n-- Enviando respuesta SETUP... --\n", 35);
            }
            else if(memcmp(respuestaRTSP.method,"PLAY",strlen(respuestaRTSP.method))==0){
                write(STDOUT_FILENO, "\n-- Enviando respuesta PLAY... --\n", 34);
            }
            else if(memcmp(respuestaRTSP.method,"TEARDOWN",strlen(respuestaRTSP.method))==0){
                write(STDOUT_FILENO, "\n-- Enviando respuesta TEARDOWN... --\n", 38);
            }
        //Si la lectura fue completa reseteo los buffers para que no queden restos de paquetes anteriores
        memset(lecturaCompleta, 0, tamBuf);
        }
        memset (buf, 0, tamBuf);
    }
    if(leido==-1)
        perror("Fracaso en el read:");
        //Cierro la conexión con el cliente
        write(STDOUT_FILENO,"Cerrando hilo TCP...",19);
        close (sd);
		pthread_exit (NULL);
}