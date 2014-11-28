
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

void atenderClienteTCP(tcp_info * clientInfo){
    
	int sd = clientInfo->sd;
	char buf[1024];
    char lecturaCompleta[2048];
    char * plecturaCompleta = lecturaCompleta;
	int tamBuf = sizeof buf;
	int leido;
    int qid;
    //hilo UDP
    pthread_t rUid;
    msg_ipc msg;

    client_packet respuestaRTSP;
    client_packet * pRTSP = &respuestaRTSP;

    //Copia la llave para IPC
    respuestaRTSP.key = clientInfo->key;
    
	memset(buf, 0, tamBuf);
    memset(lecturaCompleta, 0, tamBuf);

//====================================
    /*Empieza lectura del socket*/
//====================================

    write(STDOUT_FILENO,"\n Hilo TCP inicializado\n",24);
    while((leido = read(sd, buf, tamBuf))>0 && (memcmp(respuestaRTSP.method, "TEARDOWN", 8) != 0) ){

        respuestaRTSP = analizarRespuestaRTSP(buf, plecturaCompleta);

        //Si el mensaje es correcto le respondo sino seguiré leyendo
        if(respuestaRTSP.pckComplete == true){
            

            write(STDOUT_FILENO, "\n\n  -Client said:\n", 18);
            write(STDOUT_FILENO, lecturaCompleta, strlen(lecturaCompleta));

            construirRespuestaRTSP(pRTSP);

            write(STDOUT_FILENO, "\n  -Server said:\n", 17);
            write(STDOUT_FILENO, respuestaRTSP.body, strlen(respuestaRTSP.body));

            write(sd, respuestaRTSP.body, strlen(respuestaRTSP.body));

            //Empiezo la transmision por UDP, se deberían considerar múltiples llamadas a SETUP por el mismo cliente para que no explote
            if( memcmp(respuestaRTSP.method, "SETUP", 5) == 0){
                //Arranco el hilo UDP y no hago nada
                strcpy(respuestaRTSP.ip, clientInfo->ip);
                if( (qid = msgget(respuestaRTSP.key, IPC_CREAT | 0666)) < 0 )
                    perror("Fracaso al crear IPC para TCP");
                write(STDOUT_FILENO,"\nIPC Creado con exito", 21);

                pthread_create(&rUid, NULL, (void*)atenderClienteUDP, (void*)&respuestaRTSP);
            }
            else if( memcmp(respuestaRTSP.method, "PLAY", 4) == 0){
               //Empiezo el streaming !
                strncpy(msg.mtext, "PLAY", 4);
                msg.mtype = 1;
                if(msgsnd(qid, &msg, BUFMSG, 0) < 0)
                    perror("Fracaso al enviar IPC PLAY:");
            }
            //Kill it before it lays eggs !
            else if( memcmp(respuestaRTSP.method, "TEARDOWN", 8) == 0 ){
                #ifdef DEBUG
                    write(STDOUT_FILENO,"\n Eliminando hilo UDP", 21);
                #endif
                strncpy(msg.mtext, "TEARDOWN", 8);
                msg.mtype = 2;
                if(msgsnd(qid, &msg, BUFMSG, 0) < 0)
                    perror("Fracaso al enviar IPC TEARDOWN");
                #ifdef DEBUG
                else
                    write(STDOUT_FILENO,"\nTEARDOWN enviado con exito a UDP", 33);
                #endif

                msgctl(qid, IPC_RMID, NULL);
            }

        //Si la lectura fue completa reseteo los buffers para que no queden restos de paquetes anteriores
        memset(lecturaCompleta, 0, tamBuf);
        }
        memset (buf, 0, tamBuf);
    }

	if(memcmp(respuestaRTSP.method, "TEARDOWN", 8) == 0 )
	{
		write(STDOUT_FILENO, "\nCerrando hilo TCP por TEARDOWN", 31);
        write(STDOUT_FILENO, "\n - Conexion con el cliente terminada -", 39);
		close(sd);
		return;
	}
    if(leido==-1)
        perror("TCP->Fracaso en el read");
        //Cierro la conexión con el cliente
        write(STDOUT_FILENO, "\n - Conexion con el cliente terminada -", 39);
        close (sd);
}
