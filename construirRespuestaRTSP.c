#include "funciones.h"

//========================================================================
    /*
    Arma la respuesta desde la estructura client_packet
    */
//========================================================================

void construirRespuestaRTSP(client_packet * packetRTSP){
    //================================
        /*OPTIONS*/
    //================================
	if( memcmp(packetRTSP->method, "OPTIONS", 7) == 0 )
	{
		memcpy(packetRTSP->body, "RTSP/1.0 200 OK\r\n", 17);
		strncat(packetRTSP->body, "CSeq: ", 6);
		strncat(packetRTSP->body, packetRTSP->cseq, strlen(packetRTSP->cseq));
		strncat(packetRTSP->body, "\r\n", 2);
		strncat(packetRTSP->body, "Public: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE",46);
		strncat(packetRTSP->body, "\r\n\r\n", 4);
	}
    //================================
        /*DESCRIBE*/
    //================================
    //Necesarios: content-type (mime de accept) content-lenght
    //Despues en el body: :/ según el RFC con varios pero con 'm' y algunos 'a' la cosa debería funcionar
	else if( memcmp(packetRTSP->method, "DESCRIBE", 8) == 0 ){
		memcpy(packetRTSP->body, "RTSP/1.0 200 OK\r\n", 17);
		strncat(packetRTSP->body, "CSeq: ", 6);
		strncat(packetRTSP->body, packetRTSP->cseq, strlen(packetRTSP->cseq));
		strncat(packetRTSP->body, "\r\n", 2);
		strncat(packetRTSP->body, "Content-Type: application/sdp", 29);
		strncat(packetRTSP->body, "\r\n", 2);
		strncat(packetRTSP->body, "Content-Length: 83", 18); //Se cuenta el último CRLF pero no el primero
		strncat(packetRTSP->body, "\r\n\r\n", 4);
		strncat(packetRTSP->body, "m=video 51372 UDP 96",20); //96 es tipo dinamico, se especifica despues
		strncat(packetRTSP->body, "\r\n", 2);
		strncat(packetRTSP->body, "a=mimetype:string;\"video/H264\"",30);
		strncat(packetRTSP->body, "\r\n", 2);
		strncat(packetRTSP->body, "a=rtpmap:96 H264/23976215",25);
		strncat(packetRTSP->body, "\r\n\r\n", 4);
	}
    //================================
        /*SETUP*/
    //================================
	else if( memcmp(packetRTSP->method, "SETUP", 5) == 0 ){
		memcpy(packetRTSP->body, "RTSP/1.0 200 OK\r\n", 17);
		strncat(packetRTSP->body, "CSeq: ", 6);
		strncat(packetRTSP->body, packetRTSP->cseq, strlen(packetRTSP->cseq));
		strncat(packetRTSP->body, "\r\n", 2);
		strncat(packetRTSP->body, "Transport: UDP;unicast;client_port=51372-51373;server_port=9000-9001", 68);
		strncat(packetRTSP->body, "\r\n", 2);
		strncat(packetRTSP->body, "Session: 12345678", 17);
		strncat(packetRTSP->body, "\r\n\r\n", 4);
	}
    //================================
        /*TEARDOWN*/
    //================================
	else if( memcmp(packetRTSP->method, "TEARDOWN", 8) == 0 ){
		memcpy(packetRTSP->body, "RTSP/1.0 200 OK\r\n", 17);
		strncat(packetRTSP->body, "CSeq: ", 6);
		strncat(packetRTSP->body, packetRTSP->cseq, strlen(packetRTSP->cseq));
		strncat(packetRTSP->body, "\r\n\r\n", 4);
	}
    //================================
        /*PLAY*/
    //================================
	else if( memcmp(packetRTSP->method, "PLAY", 4) == 0 ){
		memcpy(packetRTSP->body, "RTSP/1.0 200 OK\r\n", 17);
		strncat(packetRTSP->body, "CSeq: ", 6);
		strncat(packetRTSP->body, packetRTSP->cseq, strlen(packetRTSP->cseq));
		strncat(packetRTSP->body, "\r\n", 2);
		strncat(packetRTSP->body, "Session: 12345678", 17);
		strncat(packetRTSP->body, "\r\n\r\n", 4);
	}
    //================================
        /*Not Implemented*/
    //================================
	else if( memcmp(packetRTSP->method, "Not Implemented", 15) == 0 ){
		memcpy(packetRTSP->body, "RTSP/1.0 501 Not Implemented\r\n", 30);
		strncat(packetRTSP->body, "\r\n\r\n", 4);
	}

}
