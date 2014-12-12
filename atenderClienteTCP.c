
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
        perror("FRACASO en el sem_post");


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

    write(STDOUT_FILENO,"\n Hilo TCP inicializado\n",24);
    while((leido = read(sd, buf, tamBuf))>0 && (memcmp(respuestaRTSP.method, "TEARDOWN", 8) != 0) ){
        printf("\nTest antes analiz keyQ: %s\n", clientInfo.keyQ);
        respuestaRTSP = analizarRespuestaRTSP(buf, plecturaCompleta);

        //Si el mensaje es correcto le respondo sino seguiré leyendo
        if(respuestaRTSP.pckComplete == true){
            

            write(STDOUT_FILENO, "\n\n  -Client said:\n", 18);
            write(STDOUT_FILENO, lecturaCompleta, strlen(lecturaCompleta));
            printf("\nTest despues analiz keyQ: %s\n", clientInfo.keyQ);
            construirRespuestaRTSP(pRTSP);
            printf("\nTest despues construir keyQ: %s\n", clientInfo.keyQ);
            write(STDOUT_FILENO, "\n  -Server said:\n", 17);
            write(STDOUT_FILENO, respuestaRTSP.body, strlen(respuestaRTSP.body));

            write(sd, respuestaRTSP.body, strlen(respuestaRTSP.body));

            /*
                Tomo decisiones según lo que me dijo el cliente
                DESCRIBE en construirRespuesta.c seteó method en NOT FOUND por un 404, me desconecto del cliente
                no puedo cerrarlo con un TEARDOWN porque hilo UDP aún no fue creado, es un caso especial
            */
            /*if( (memcmp(respuestaRTSP.method, "NOT FOUND", 9) == 0) ){

            }
            else */if( (memcmp(respuestaRTSP.method, "SETUP", 5) == 0) && (!setup) ){
                //Arranco el hilo UDP y no hago nada
                strcpy(respuestaRTSP.ip, clientInfo.ip);
                #ifdef DEBUG
                    printf("\nTCP->Voy a abrir con clientInfo.key: %s\n", clientInfo.keyQ);
                #endif

                // Creo la cola de mensajes
                if( (mq = mq_open(clientInfo.keyQ, O_CREAT | O_RDWR, 0666, &ma)) < 0)
                    perror("\nFRACASO queue_open");
                #ifdef DEBUG
                    write(STDOUT_FILENO,"\nIPC Creado con exito", 21);
                #endif
                memset(respuestaRTSP.keyQ, 0, sizeof(respuestaRTSP.keyQ));
                strncpy(respuestaRTSP.keyQ, clientInfo.keyQ, strlen(clientInfo.keyQ) );
                pthread_create(&rUid, NULL, (void*)atenderClienteUDP, (void*)&respuestaRTSP);
                setup = true;
            }
            else if( memcmp(respuestaRTSP.method, "PLAY", 4) == 0){
               //Empiezo el streaming !
		      write(STDOUT_FILENO, "\nTCP->Enviando play", 19);
                strncpy(msg, "PLAY", 4);
                if(mq_send(mq, msg, 4, 0)<0)
                    perror("\nFRACASO en el mq_send");  

            }
            //Kill it before it lays eggs !
            else if( memcmp(respuestaRTSP.method, "TEARDOWN", 8) == 0 ){
                #ifdef DEBUG
                    write(STDOUT_FILENO,"\n Eliminando hilo UDP", 21);
                #endif
                strncpy(msg, "TEARDOWN", 8);

                if(mq_send(mq, msg, 8, 0)<0)
                    perror("\nFRACASO en el mq_send");  

                #ifdef DEBUG
                else
                    write(STDOUT_FILENO,"\nTEARDOWN enviado con exito a UDP", 33);
                #endif
                //Cierro la cola y elimino su identificador
                mq_close(mq);
                if(mq_unlink(clientInfo.keyQ)<0)
                    perror("FRACASO en unlink");
            }

        //Si la lectura fue completa reseteo los buffers para que no queden restos de paquetes anteriores
        memset(lecturaCompleta, 0, tamBuf);
        }
        memset(buf, 0, tamBuf);
    }

	if(memcmp(respuestaRTSP.method, "TEARDOWN", 8) == 0 )
	{
		write(STDOUT_FILENO, "\nCerrando hilo TCP por TEARDOWN", 31);
	}
    else if(leido==-1)
        perror("TCP->FRACASO en el read");
        //Cierro la conexión con el cliente
        write(STDOUT_FILENO, "\n - Conexion con el cliente terminada -", 39);
        close (sd);
}
