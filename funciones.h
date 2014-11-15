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
void atenderClienteTCP(int * socketD);
void atenderClienteUDP(int * socketD);

typedef struct CLIENT_PACKET{
	char header[256];
	char body[1024];
	char method[20];
	int cseq;
	bool pckComplete;
} client_packet;

client_packet analizarRespuestaRTSP(char lectura[1024], char * lecturaRTSP);

#endif