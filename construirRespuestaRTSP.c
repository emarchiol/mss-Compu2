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
	else if( memcmp(packetRTSP->method, "DESCRIBE", 8) == 0 ){
		memcpy(packetRTSP->body, "RTSP/1.0 200 OK\r\n", 17);
		strncat(packetRTSP->body, "CSeq: ", 6);
		strncat(packetRTSP->body, packetRTSP->cseq, strlen(packetRTSP->cseq));
		strncat(packetRTSP->body, "\r\n", 2);
		strncat(packetRTSP->body, "BASURA", 6);
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
		strncat(packetRTSP->body, "BASURA", 6);
		strncat(packetRTSP->body, "\r\n\r\n", 4);
	}
    //================================
        /*TEARDOWN*/
    //================================
	else if( memcmp(packetRTSP->method, "TEARDOWN", 8) == 0 ){
		memcpy(packetRTSP->body, "RTSP/1.0 200 OK\r\n", 17);
		strncat(packetRTSP->body, "CSeq: ", 6);
		strncat(packetRTSP->body, packetRTSP->cseq, strlen(packetRTSP->cseq));
		strncat(packetRTSP->body, "\r\n", 2);
		strncat(packetRTSP->body, "BASURA", 6);
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
		strncat(packetRTSP->body, "BASURA", 6);
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
