#ifndef _FUNCIONES_H_
	#define _FUNCIONES_H_
	
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <netdb.h>
	#include <string.h>
	#include <errno.h>
	#include <stdlib.h>
	#include <pthread.h>
	#include <semaphore.h>
	#include <stdbool.h>

typedef struct CLIENT_PACKET{
	char header[256];
	char body[1024];
	char method[20];
	char cseq[4];
	char uri[128];
	bool pckComplete;
	char fileToPlay[128];
	//DESCRIBE
	char contentType[64]; 	//Mime type
	char contentLenght[5];	//Tamaño de todo lo de abajo
	char v[3]; //Protocol version
	char o[64]; //Dunno
	char s[64]; //Session name
	char t[64]; //Time the session is active
	char m[64]; //Media name and transport address
} client_packet;

typedef struct TCP_INFO {
    int sd;
    char ip[15];
} tcp_info;

void atenderClienteTCP(tcp_info * clientInfo);
void atenderClienteUDP(int * socketD, client_packet packetRTSP);
void construirRespuestaRTSP(client_packet * packet);
client_packet analizarRespuestaRTSP(char lectura[1024], char * lecturaRTSP);

#endif