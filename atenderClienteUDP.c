//================================================================================================
    /* 
    El hilo UDP creado por TCP se encargar de enviar paquetes RTP al ip del cliente espcificado 
    en la estructura "packetRTSP" al puerto UDPPORT especificado en funciones.h, tenicamente el 
    puerto debería ser variable, pero ya que es el servidor el que lo especifica lo mantengo 
    constante por ahora. Hilo UDP lee mensajes desde hilo TCP para saber cuando terminar su 
    trabajo, pausarlo o realizar un "seek".
    */
//================================================================================================
#include "funciones.h"

char * rtpPacket(char buf[512]);
char RtpBuf[PACKETSIZE];
void atenderClienteUDP(client_packet * packetRTSP){

#ifdef DEBUG
    write(STDOUT_FILENO, "\nUDP->Hilo UDP Inicializado", 27);
    write(STDOUT_FILENO, "\nUDP->IP:", 9);
#endif

    write(STDOUT_FILENO, packetRTSP->ip, strlen(packetRTSP->ip));
    int fd; //file descriptor para abrir el archivo multimedia
    int leido;
    bool teardown = false;
    bool pause = false;
    //Tiene que crear un nuevo socket con otro puerto previamente establecido mediante SETUP y desde server.c se tiene que pasar la IP del cliente (tmb el puerto)
    int sdUDP;
    char buf[512]; //Payload de video
    char completePacket[PACKETSIZE]; //Paquete completo
    int tamBuf = sizeof(buf);
    struct sockaddr_in address;
    //"leido" para colas
    int rcv;
    //Descriptor de la cola
    mqd_t mq;
    //estructura de la cola
    struct mq_attr ma;
    char msg[QBUFMSG];
    // Atributos de la cola
    ma.mq_flags = QNOFLAGS;
    ma.mq_maxmsg = QMAX;
    ma.mq_msgsize = QBUFMSG;
    ma.mq_curmsgs = QNOFLAGS;
      
    //Inicia la estructura de direcciones de socket para los protocolos de internet
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, packetRTSP->ip, &address.sin_addr );
    //htons convierte el puerto string a puerto inet
    address.sin_port = htons(UDPPORT);

    /*Creamos el socket UDP*/
    sdUDP = socket(AF_INET, SOCK_DGRAM, 0);
    //Abro IPC
    #ifdef DEBUG
    printf("\nUDP->Voy a abrir con clientInfo.key: %s\n", packetRTSP->keyQ);
    #endif
    if( (mq = mq_open(packetRTSP->keyQ, O_RDONLY | O_NONBLOCK, 0644, &ma)) < 0){
        perror("\nUDP->Ups, queue_open dijo");
        return;
    }
    #ifdef DEBUG 
    else
        write(STDOUT_FILENO, "\nUDP->IPC Abierto con exito", 27);
    #endif
    while( !teardown ){  
        
        //Para imprimir perror debería hacer un analisis del error de otro modo al ser O_NONBLOCK spamearía todo el tiempo
        rcv = mq_receive(mq, msg , BUFMSG, 0);
            /*perror("\nFRACASO en el mq_rcv"); 
            //solo para verificar que al apretar "STOP" hilo UDP realmente lo recibe y para el streaming
            sleep(0); 
        */
        if(strncmp(msg, "TEARDOWN", rcv) == 0)
            teardown = true;
        
        if( strncmp(msg, "PLAY", rcv) == 0 ){

            // Abre un archivo y lo envía por el socket
            if ((fd = open (packetRTSP->fileToPlay, O_RDONLY)) < 0)
            {
                perror("UDP->Ups, open dijo");
                exit(1);
            }

            memset(buf, 0, tamBuf);
            write(STDOUT_FILENO, "\nUDP->Streaming...", 18);
            //Se envia el archivo mientras haya lectura y teardown O pause sean falsos
            while ( ((leido = read(fd, buf, tamBuf)) > 0) && ((!teardown) && (!pause)) )
            {
                //Pregunto si durante el PLAY se envió otra señal, si se hizo es porque tengo que dejar de enviar el archivo
                if( (rcv = mq_receive(mq, msg , BUFMSG, 0)) > 0){
                    #ifdef DEBUG
                        write(STDOUT_FILENO, "\nUDP->Recibi algo", 17);
                    #endif
                    //Debería haber otro para PLAY, en caso de que se haga un "seek"
                    //PAUSE Not Implemented
                    if( strncmp(msg, "PAUSE", rcv) == 0 ){
                        pause = true;
                        #ifdef DEBUG
                            write(STDOUT_FILENO, "\nUDP->PAUSE", 11);
                        #endif
                    }
                    else if( strncmp(msg, "TEARDOWN", rcv) == 0 )
                    {
                        teardown = true;
                        #ifdef DEBUG
                            write(STDOUT_FILENO, "\nUDP->TEARDOWN", 14);
                        #endif
                            //Destruyo el IPC
                            if(mq_close(mq)<0)
                                perror("UDP->Ups, mq_close dijo");
                    }
                }else
                {
                    //Envio de datos
                    memcpy(completePacket, rtpPacket(buf), PACKETSIZE);
                    sendto(sdUDP, completePacket, sizeof(completePacket), 0, (struct sockaddr *)&address, sizeof(address));
                    memset (buf, 0, tamBuf);
                }
            }
            write(STDOUT_FILENO, "\nUDP->Done", 10);
            teardown = true;
        }
        memset(buf, 0, tamBuf);
    }

    //Cierro descriptores
    if(close (fd)<0)
        perror("UDP->Ups, close dijo");
    if(close(sdUDP)<0)
        perror("UDP->Ups, close dijo");
    #ifdef DEBUG
        write(STDOUT_FILENO, "\nUDP->Hilo UDP finalizado\n", 26);
    #endif
}

char * rtpPacket(char buf[512]){
    
    #define KRtpHeaderSize 12           // size of the RTP header
    #define KJpegHeaderSize 8           // size of the special JPEG payload header

    
    int RtpPacketSize = KRtpHeaderSize + KJpegHeaderSize;
    int m_Timestamp = 0;
    int m_SequenceNumber = 0;

    memset(RtpBuf,0x00,sizeof(RtpBuf));
    // Prepare the first 4 byte of the packet. This is the Rtp over Rtsp header in case of TCP based transport
    RtpBuf[0]  = '$';        // magic number (??)
    RtpBuf[1]  = 0;          // number of multiplexed subchannel on RTSP connection - here the RTP channel (wireshark dice: Channel:)
    RtpBuf[2]  = (RtpPacketSize & 0x0000FF00) >> 8; //Length, 2bytes
    RtpBuf[3]  = (RtpPacketSize & 0x000000FF);
    // Prepare the 12 byte RTP header
    RtpBuf[4]  = 0x80;                               // RTP version -> 128 -> binary 10
    RtpBuf[5]  = 0x9a;                               // JPEG payload (26) and marker bit
    RtpBuf[7]  = m_SequenceNumber & 0x0FF;           // each packet is counted with a sequence counter
    RtpBuf[6]  = m_SequenceNumber >> 8;
    RtpBuf[8]  = (m_Timestamp & 0xFF000000) >> 24;   // each image gets a timestamp
    RtpBuf[9]  = (m_Timestamp & 0x00FF0000) >> 16;
    RtpBuf[10] = (m_Timestamp & 0x0000FF00) >> 8;
    RtpBuf[11] = (m_Timestamp & 0x000000FF);
    RtpBuf[12] = 0x13;                               // 4 byte SSRC (sychronization source identifier)
    RtpBuf[13] = 0xf9;                               // we just an arbitrary number here to keep it simple
    RtpBuf[14] = 0x7e;
    RtpBuf[15] = 0x67;
    // Prepare the 8 byte payload JPEG header
    RtpBuf[16] = 0x00;                               // type specific
    RtpBuf[17] = 0x00;                               // 3 byte fragmentation offset for fragmented images
    RtpBuf[18] = 0x00;
    RtpBuf[19] = 0x00;
    RtpBuf[20] = 0x01;                               // type
    RtpBuf[21] = 0x5e;  

    RtpBuf[22] = 0x08;                           // width  / 8 -> 64 pixel
    RtpBuf[23] = 0x06;                           // height / 8 -> 48 pixel
    
    // append the JPEG scan data to the RTP buffer
    memcpy(&RtpBuf[24], buf, 512);
    
    m_SequenceNumber++;                              // prepare the packet counter for the next packet
    m_Timestamp += 3600;                             // fixed timestamp increment for a frame rate of 25fps

    return RtpBuf;
}