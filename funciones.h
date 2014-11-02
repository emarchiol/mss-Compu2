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
void atenderClienteTCP(int * socketD);
void atenderClienteUDP(int * socketD);
#endif