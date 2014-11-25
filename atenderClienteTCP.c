
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

    //hilo UDP
    //pthread_t rUid;

    client_packet respuestaRTSP;
    client_packet * pRTSP = &respuestaRTSP;
    
	memset(buf, 0, tamBuf);
    memset(lecturaCompleta, 0, tamBuf);

//====================================
    /*Empieza lectura del socket*/
//====================================

    write(STDOUT_FILENO,"\n Hilo TCP inicializado\n",24);
    while((leido = read(sd, buf, tamBuf))>0){


        respuestaRTSP = analizarRespuestaRTSP(buf, plecturaCompleta);

        //Si el mensaje es correcto le respondo sino seguiré leyendo
        if(respuestaRTSP.pckComplete == true){
            
            write(STDOUT_FILENO, "\n  -Client said:\n", 17);
            write(STDOUT_FILENO, lecturaCompleta, strlen(lecturaCompleta));

            construirRespuestaRTSP(pRTSP);

            write(STDOUT_FILENO, "\n  -Server said:\n", 17);
            write(STDOUT_FILENO, respuestaRTSP.body, strlen(respuestaRTSP.body));

            write(sd, respuestaRTSP.body, strlen(respuestaRTSP.body));

            //Empiezo la transmision por UDP
            if(memcmp(respuestaRTSP.method, "PLAY", 4)){
                //Leo desde el protocolo UDP, esta linea probablemente irá en atenderClienteTCP.c
                //pthread_create(&rUid, NULL, (void*)atenderClienteUDP, (void*)&client_sd);
            }
            //Kill it before it lays eggs !
            else if( memcmp(respuestaRTSP.method, "TEARDOWN", 8) ){

            }

        //Si la lectura fue completa reseteo los buffers para que no queden restos de paquetes anteriores
        memset(lecturaCompleta, 0, tamBuf);
        }
        memset (buf, 0, tamBuf);
    }
    if(leido==-1)
        perror("Fracaso en el read");
        //Cierro la conexión con el cliente
        write(STDOUT_FILENO, "\nCerrando hilo TCP...", 20);
        close (sd);
		pthread_exit (NULL);
}
