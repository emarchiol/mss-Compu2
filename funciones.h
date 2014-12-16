#ifndef _FUNCIONES_H_
	#define _FUNCIONES_H_
	
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/ipc.h>
	#include <sys/msg.h>
	#include <arpa/inet.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <netdb.h>
	#include <string.h>
	#include <errno.h>
	#include <stdlib.h>
	#include <pthread.h>
	#include <stdbool.h>
	#include <mqueue.h>
	#include <signal.h>
	#include <time.h>
	#include <semaphore.h>

	//Constantes
	#define QBUFMSG 16
	#define BUFMSG 16
	#define QNOFLAGS 0
	#define QMAX 5
	#define UDPPORT 51372
	#define PACKETSIZE 2048

	//Semaforo
	sem_t *semaforo;

typedef struct CLIENT_PACKET{
	char body[512];
	char content[512];
	char method[20];
	char cseq[4];
	char uri[128];
	bool pckComplete;
	char fileToPlay[128];
	//DESCRIBE
	char contentType[64]; 	//Mime type
	char contentLength[5];	//Tamaño de todo lo de abajo
	char v[16]; //Protocol version
	char m[64]; //Media name and transport address
	char aMime[64]; //Media name and transport address
	char aRTPMap[64]; //Media name and transport address
	char aSize[32]; //Media name and transport address
	char aRate[32]; //Media name and transport address
	char ip[15]; //Para el protocolo UDP
	char keyQ[8]; //Otra vez, el identificador de cola para que sea transportado a hilo UDP
} client_packet;

typedef struct TCP_INFO {
    int sd;
    char ip[15];
    //IPC: identificador de de cola creado por server
    char keyQ[8];
} tcp_info;

void atenderClienteTCP(tcp_info * clientInfo);
void atenderClienteUDP(client_packet * packetRTSP);
void construirRespuestaRTSP(client_packet * packet);
client_packet analizarRespuestaRTSP(char lectura[1024], char * lecturaRTSP);

#endif