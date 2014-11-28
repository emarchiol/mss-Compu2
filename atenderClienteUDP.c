//================================================================================================
    /* 
    El hilo UDP creado por TCP se encargar de enviar paquetes RTP al ip del cliente espcificado 
    en la estructura "packetRTSP" al puerto UDPPORT especificado en funciones, tenicamente el 
    puerto debería ser variable, pero ya que es el servidor el que lo especifica lo mantengo 
    constante por ahora. Hilo UDP lee mensajes desde hilo TCP para saber cuando terminar su trabajo,
    pausarlo o realizar un "seek".
    */
//================================================================================================
#include "funciones.h"

void atenderClienteUDP(client_packet * packetRTSP){
#ifdef DEBUG
    write(STDOUT_FILENO, "\nUDP->Hilo UDP Inicializado", 27);
    write(STDOUT_FILENO, "\nUDP->IP: ", 10);
#endif

    write(STDOUT_FILENO, packetRTSP->ip, strlen(packetRTSP->ip));

    int fd; //file descriptor para abrir el archivo multimedia
    int leido;
    bool teardown = false;
    bool pause = false;
    //Tiene que crear un nuevo socket con otro puerto previamente establecido mediante SETUP y desde server.c se tiene que pasar la IP del cliente
    //Tiene que recibir por parametro la estructura previamente y la IP (tmb necesita puerto)
    int sdUDP;
    char buf[512];
    int tamBuf = sizeof(buf);
    struct sockaddr_in address;
    int qid;
    int rcv;
    msg_ipc msg;

    //Inicia la estructura de direcciones de scoket para los protocolos de internet

    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, packetRTSP->ip, &address.sin_addr );
    //htons convierte el puerto string a puerto inet
    address.sin_port = htons(UDPPORT);

    /*Creamos el socket UDP*/

    sdUDP = socket(AF_INET, SOCK_DGRAM, 0);
    qid = msgget(packetRTSP->key, IPC_CREAT | 0666);

while( !teardown ){  
    
    write(STDOUT_FILENO, "\nUDP->Escuchando IPC's", 22);

    if( (rcv = msgrcv(qid, &msg, BUFMSG, 0, 0)) < 0 )
        perror("Fracaso en el IPC UDP");

    #ifdef DEBUG
        write(STDOUT_FILENO, "\nUDP->Recibi un:", 16);
        write(STDOUT_FILENO, msg.mtext, rcv);
        write(STDOUT_FILENO, "<-", 2);
    #endif

    if(strncmp(msg.mtext, "TEARDOWN", rcv) == 0)
        teardown = true;
    
    if( strncmp(msg.mtext, "PLAY", rcv) == 0 ){
        
        // Abre un archivo y lo envía por el socket
        if ((fd = open (packetRTSP->fileToPlay, O_RDONLY)) < 0)
        {
            perror("UDP->Fracaso en abrir el archivo a reproducir, open dijo");
            exit(1);
        }

        memset(buf, 0, tamBuf);
        write(STDOUT_FILENO, "\nUDP->Enviando archivo por UDP...", 33);
        //Se envia el archivo mientras haya lectura y teardown O pause sean falsos
        while ( ((leido = read(fd, buf, tamBuf)) > 0) && ((!teardown) && (!pause)) )
        {
            //Pregunto si durante el PLAY se envió otra señal, si se hizo es porque tengo que dejar de enviar el archivo
            if( (rcv = msgrcv(qid, &msg, BUFMSG, 0, IPC_NOWAIT)) > 0)
            {
                #ifdef DEBUG
                    write(STDOUT_FILENO, "\nUDP->Recibi algo", 17);
                #endif
                //Debería haber otro para PLAY, en caso de que se haga un "seek"
                //PAUSE Not Implemented
                if( strncmp(msg.mtext, "PAUSE", rcv) == 0 ){
                    pause = true;
                    #ifdef DEBUG
                        write(STDOUT_FILENO, "\nUDP->PAUSE", 11);
                    #endif
                }
                else if( strncmp(msg.mtext, "TEARDOWN", rcv) == 0 )
                {
                    teardown = true;
                    #ifdef DEBUG
                        write(STDOUT_FILENO, "\nUDP->TEARDOWN", 14);
                    #endif
                }
            }else
            {
                sendto(sdUDP, buf, tamBuf, 0, (struct sockaddr *)&address, sizeof(address));
                //leido = write (sdUDP, buf, leido);
                memset (buf, 0, tamBuf);
                #ifdef DEBUG
                write(STDOUT_FILENO, "\nUDP->Durmiendo", 15);
                sleep(1);
                #endif
            }
        }
        
        #ifdef DEBUG
        //Espero xq ya termine
        write(STDOUT_FILENO, "\nUDP->Ya termine", 16);
        #endif
    }
    memset(buf, 0, tamBuf);
}

#ifdef DEBUG
    write(STDOUT_FILENO, "\nUDP->Recibi por ultimo:", 24);
    write(STDOUT_FILENO, buf, strlen(buf));
#endif

    //Cierro archivo
    close (fd);
    //Cierro el socket
    close(sdUDP);
    write(STDOUT_FILENO, "\nUDP->Hilo UDP finalizado\n", 26);
}
