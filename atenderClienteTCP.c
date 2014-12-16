
//========================================================================
    /*
    Este hilo se dispara cuando un cliente se conecta al servidor por
    server.c, se encarga de leer lo que el cliente envía y de responder 
    según corresponda solo si el mensaje es completo en su formato, de otro
    modo sigue leyendo del socket.

    buf es el contenido de la lectura que envió el cliente hasta un doble
    CRLF, lecturaCompleta se va llenando con buf representando así un
    paquete RTSP completo.
    */
//========================================================================
#include "funciones.h"

void atenderClienteTCP(tcp_info * cf){
    
    //Me copio la estructura en estructura local y sigo
    tcp_info clientInfo = *cf;
    if(sem_post(semaforo)<0)
        perror("TCP->Ups, sem_post dijo");


	int sd = clientInfo.sd;
	char buf[1024];
    char lecturaCompleta[2048];
    char * plecturaCompleta = lecturaCompleta;
	int tamBuf = sizeof buf;
	int leido;
    //hilo UDP
    pthread_t rUid;

    client_packet respuestaRTSP;
    client_packet * pRTSP = &respuestaRTSP;

    //PARA COLAS
    mqd_t mq;
    // initialize the queue attributes
    struct mq_attr ma;

    // Specify message queue attributes.
    ma.mq_flags = QNOFLAGS;                // blocking read/write
    ma.mq_maxmsg = QMAX;                 // maximum number of messages allowed in queue
    ma.mq_msgsize = QBUFMSG;
    ma.mq_curmsgs = QNOFLAGS;             // number of messages currently in queue

    char msg[QBUFMSG];
    
	memset(buf, 0, tamBuf);
    memset(lecturaCompleta, 0, tamBuf);
    bool setup = false;

//====================================
    /*Empieza lectura del socket*/
//====================================
    #ifdef DEBUG
        write(STDOUT_FILENO,"\n Hilo TCP inicializado\n",24);
    #endif
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


            if( (memcmp(respuestaRTSP.method, "SETUP", 5) == 0) && (!setup) ){
                //Arranco el hilo UDP y no hago nada
                strcpy(respuestaRTSP.ip, clientInfo.ip);

                // Creo la cola de mensajes
                if( (mq = mq_open(clientInfo.keyQ, O_CREAT | O_RDWR, 0666, &ma)) < 0)
                    perror("\nTCP->Ups, queue_open dijo");
                #ifdef DEBUG
                    write(STDOUT_FILENO,"\nIPC Creado con exito", 21);
                #endif
                memset(respuestaRTSP.keyQ, 0, sizeof(respuestaRTSP.keyQ));
                strncpy(respuestaRTSP.keyQ, clientInfo.keyQ, strlen(clientInfo.keyQ) );
                if( (pthread_create(&rUid, NULL, (void*)atenderClienteUDP, (void*)&respuestaRTSP)) < 0)
                    perror("TCP->Ups, pthread_create dijo");
                setup = true;
            }
            else if( memcmp(respuestaRTSP.method, "PLAY", 4) == 0){
               //Empiezo el streaming !
              #ifdef DEBUG
		      write(STDOUT_FILENO, "\nTCP->Enviando play", 19);
              #endif
                strncpy(msg, "PLAY", 4);
                if(mq_send(mq, msg, 4, 0)<0)
                    perror("\nTCP->Ups, el mq_send dijo");  

            }
            //Kill it before it lays eggs !
            else if( memcmp(respuestaRTSP.method, "TEARDOWN", 8) == 0 ){
                #ifdef DEBUG
                    write(STDOUT_FILENO,"\nTCP->Eliminando hilo UDP", 21);
                #endif
                strncpy(msg, "TEARDOWN", 8);

                if(mq_send(mq, msg, 8, 0)<0)
                    perror("\nTCP->Ups, mq_send dijo");  

                #ifdef DEBUG
                else
                    write(STDOUT_FILENO,"\nTEARDOWN enviado con exito a UDP", 33);
                #endif
                //Cierro la cola y elimino su identificador
                if(mq_close(mq)<0)
                    perror("\nUps, mq_close dijo");
                if(mq_unlink(clientInfo.keyQ)<0)
                    perror("TCP->Ups, unlink dijo");
            }

        //Si la lectura fue completa reseteo los buffers para que no queden restos de paquetes anteriores
        memset(lecturaCompleta, 0, tamBuf);
        }
        memset(buf, 0, tamBuf);
    }
    //Es solo informativo
    #ifdef DEBUG
	if(memcmp(respuestaRTSP.method, "TEARDOWN", 8) == 0 )
	{
		write(STDOUT_FILENO, "\nCerrando hilo TCP por TEARDOWN", 31);
	}
    #endif
    if(leido==-1)
        perror("TCP->Ups, read dijo");

        //Cierro la conexión con el cliente
        write(STDOUT_FILENO, "\n - Conexion con el cliente terminada -", 39);
        if(close (sd)<0)
            perror("TCP->Ups, close dijo");
}
