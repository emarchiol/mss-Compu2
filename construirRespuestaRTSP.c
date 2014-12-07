#include "funciones.h"

//=================================================================================
    /*
    Arma la respuesta desde la estructura client_packet (mucho parseo aburrido)
    */
//=================================================================================
bool fileNotFound(client_packet * packetRTSP);
void construirRespuestaRTSP(client_packet * packetRTSP){

	//Pregunto si el archivo que me pasaron a reproducir existe realmente
    //================================
        /*200 OK OPTIONS*/
    //================================
	if( memcmp(packetRTSP->method, "OPTIONS", 7) == 0 )
	{
		memcpy(packetRTSP->body, "RTSP/1.0 200 OK\r\n", strlen("RTSP/1.0 200 OK\r\n") +1);
		strncat(packetRTSP->body, "CSeq: ", 6);
		strncat(packetRTSP->body, packetRTSP->cseq, strlen(packetRTSP->cseq));
		strncat(packetRTSP->body, "\r\n", 2);

        //Abro el archivo con los tipos de métodos RTSP implementados por el servidor 
        int fd;
        int leido;
        char buf[80];

		strncat(packetRTSP->body, "Public: ",8);

        if ((fd = open ("config/metodos", O_RDONLY)) < 0)
        {
            perror("Fracaso en abrir el archivo de metodos, open dijo:");
            exit(1);
        }

        while ( ( leido = read(fd, buf, sizeof buf)) > 0)
		{
			const char s[1] = "\n";
			char *token;

			/* get the first token */
			token = strtok(buf, s);
			/* walk through other tokens */
			while( token != NULL ) 
			{
				strncat(packetRTSP->body, token, strlen(token));
				
				token = strtok(NULL, s);
				if(token != NULL)
					strncat(packetRTSP->body, ", ", 2);
			}
		}
		strncat(packetRTSP->body, "\r\n\r\n", 4);
	}
    //================================
        /*200 OK DESCRIBE*/
    //================================
    //Necesarios: content-type (mime de accept) content-lenght
    //Despues en el body: :/ según el RFC con varios pero con 'm' y algunos 'a' la cosa funciona
	else if( memcmp(packetRTSP->method, "DESCRIBE", 8) == 0 ){

		//Primero me fijo si el archivo solicitado existe. 404 ?
		if (!fileNotFound(packetRTSP)){

			/*Construcción del contenido del mensaje*/
			memcpy(packetRTSP->v, "v=0\r\n", strlen("v=0\r\n")+1);
			memcpy(packetRTSP->m, "m=video 51372 UDP 26\r\n", strlen("m=video 51372 UDP 26\r\n")+1); //El primer número corresponde al puerto que usará el cliente, 26 es tipo mJPEG
			memcpy(packetRTSP->aMime, "a=mimetype:string;\"video/JPEG\"\r\n", strlen("a=mimetype:string;\"video/JPEG\"\r\n")+1);
			memcpy(packetRTSP->aRTPMap, "a=rtpmap:26 JPEG/90000\r\n", strlen("a=rtpmap:26 JPEG/90000\r\n")+1);
			memcpy(packetRTSP->aSize, "a=x-dimensions:384,144\r\n", strlen("a=x-dimensions:384,144\r\n")+1);
			memcpy(packetRTSP->aRate, "a=x-framerate:25\r\n", strlen("a=x-framerate:25\r\n")+1);

			strncat(packetRTSP->content, packetRTSP->v, strlen(packetRTSP->v));
			strncat(packetRTSP->content, packetRTSP->m, strlen(packetRTSP->m));
			strncat(packetRTSP->content, packetRTSP->aMime, strlen(packetRTSP->aMime));
			strncat(packetRTSP->content, packetRTSP->aRTPMap, strlen(packetRTSP->aRTPMap));
			strncat(packetRTSP->content, packetRTSP->aSize, strlen(packetRTSP->aSize));
			strncat(packetRTSP->content, packetRTSP->aRate, strlen(packetRTSP->aRate));

			strncat(packetRTSP->content, "\r\n", 2);

			//Cuantos caracteres tiene content + el último doble CRLF y no el primero
			sprintf(packetRTSP->contentLength, "%zd", strlen(packetRTSP->content));

			/*Construcción del mensaje completo*/
			memcpy(packetRTSP->body, "RTSP/1.0 200 OK\r\n", strlen("RTSP/1.0 200 OK\r\n")+1);
			strncat(packetRTSP->body, "CSeq: ", 6);
			strncat(packetRTSP->body, packetRTSP->cseq, strlen(packetRTSP->cseq));
			strncat(packetRTSP->body, "\r\n", 2);
			strncat(packetRTSP->body, "Content-Type: application/sdp", 29);
			strncat(packetRTSP->body, "\r\n", 2);
			strncat(packetRTSP->body, "Content-Length: ", 16);
			strcat(packetRTSP->body, packetRTSP->contentLength);
			strncat(packetRTSP->body, "\r\n\r\n", 4);
			strncat(packetRTSP->body, packetRTSP->content, strlen(packetRTSP->content));
		}
	}
    //================================
        /*200 OK SETUP*/
    //================================
	else if( memcmp(packetRTSP->method, "SETUP", 5) == 0 ){
		memcpy(packetRTSP->body, "RTSP/1.0 200 OK\r\n", strlen("RTSP/1.0 200 OK\r\n")+1);
		strncat(packetRTSP->body, "CSeq: ", 6);
		strncat(packetRTSP->body, packetRTSP->cseq, strlen(packetRTSP->cseq));
		strncat(packetRTSP->body, "\r\n", 2);
		strncat(packetRTSP->body, "Transport: UDP;unicast;client_port=51372-51373;server_port=9000-9001", 68);
		strncat(packetRTSP->body, "\r\n", 2);
		strncat(packetRTSP->body, "Session: 12345678", 17);
		strncat(packetRTSP->body, "\r\n\r\n", 4);
	}
    //================================
        /*200 OK TEARDOWN*/
    //================================
	else if( memcmp(packetRTSP->method, "TEARDOWN", 8) == 0 ){
		memcpy(packetRTSP->body, "RTSP/1.0 200 OK\r\n", strlen("RTSP/1.0 200 OK\r\n")+1);
		strncat(packetRTSP->body, "CSeq: ", 6);
		strncat(packetRTSP->body, packetRTSP->cseq, strlen(packetRTSP->cseq));
		strncat(packetRTSP->body, "\r\n\r\n", 4);
	}
    //================================
        /*200 OK PLAY*/
    //================================
	else if( memcmp(packetRTSP->method, "PLAY", 4) == 0 ){
		
		//Primero me fijo si el archivo solicitado existe. 404 ?
		if (!fileNotFound(packetRTSP)){
			memcpy(packetRTSP->body, "RTSP/1.0 200 OK\r\n", strlen("RTSP/1.0 200 OK\r\n")+1);
			strncat(packetRTSP->body, "CSeq: ", 6);
			strncat(packetRTSP->body, packetRTSP->cseq, strlen(packetRTSP->cseq));
			strncat(packetRTSP->body, "\r\n", 2);
			strncat(packetRTSP->body, "Session: 12345678", 17);
			strncat(packetRTSP->body, "\r\n\r\n", 4);
		}
	}
    //================================
        /*501 Not Implemented*/
    //================================
	else if( memcmp(packetRTSP->method, "Not Implemented", 15) == 0 ){
		memcpy(packetRTSP->body, "RTSP/1.0 501 Not Implemented\r\n", strlen("RTSP/1.0 501 Not Implemented\r\n")+1 );
		strncat(packetRTSP->body, "\r\n\r\n", 4);
	}
	//================================
        /*400 Bad Request*/
    //================================
	else if( memcmp(packetRTSP->method, "Bad Request", 11) == 0 ){
		memcpy(packetRTSP->body, "RTSP/1.0 400 Bad Request\r\n", strlen("RTSP/1.0 400 Bad Request\r\n")+1 );
		strncat(packetRTSP->body, "\r\n\r\n", 4);
	}
}
    //================================
        /*404 Not Found*/
    //================================

bool fileNotFound(client_packet * packetRTSP){
	int fd;
	if ((fd = open (packetRTSP->fileToPlay, O_RDONLY)) < 0 )
	{
		memcpy(packetRTSP->body, "RTSP/1.0 404 Not Found\r\n", strlen("RTSP/1.0 404 Not Found\r\n")+1 );
		strncat(packetRTSP->body, "CSeq: ", 6);
		strncat(packetRTSP->body, packetRTSP->cseq, strlen(packetRTSP->cseq));
		strncat(packetRTSP->body, "\r\n", 2);
		strncat(packetRTSP->body, "\r\n\r\n", 4);
		close(fd);
		return true;
	}
	else return false;
}