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
	//Solo para pthread_kill, debería eliminarse
	#include <signal.h>
	#define BUFMSG 16

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
	//Para el protocolo UDP
	char ip[15];
	key_t key; //qid en realidad es para el IPC, no tiene mucho que ver con esta estructura, pero es la manera más sencilla de pasarle el id

	#define UDPPORT 51372
} client_packet;

typedef struct TCP_INFO {
    int sd;
    char ip[15];
    //IPC: el identificador de cola no escreado acá xq no es necesario crear la cola acá, solo es necesario entregar una key única a cada hiloTCP
    key_t key;
} tcp_info;

typedef struct MSG_IPC{
	long mtype;
	char mtext[BUFMSG];
}msg_ipc;

void atenderClienteTCP(tcp_info * clientInfo);
void atenderClienteUDP(client_packet * packetRTSP);
void construirRespuestaRTSP(client_packet * packet);
client_packet analizarRespuestaRTSP(char lectura[1024], char * lecturaRTSP);

#endif