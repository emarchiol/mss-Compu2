#include "funciones.h"

//====================================================================================
    /*
    Se encarga de analizar las lecturas que provienen del cliente, 
    verificar que estén completas y ordenarlas dentro de la estructura
    correspondiente. Recibe "lectura" que representa el mensaje parcial o completo
    del cliente, en caso de no ser completo lo va construyendo sobre "lecturaCompleta"
    Una vez completo el mensaje ordena el mismo en la estructura que devuelve.
    */
//====================================================================================

client_packet analizarRespuestaRTSP(char lectura[1024], char * lecturaCompleta){

	char dobleCRLF[4]= "\r\n\r\n";
	char buf[128];
    char token[128]; //Se usa para el parseo desde archivos/paquetes
    char * pToken = malloc(sizeof(char)*128);
    char * aux = malloc(sizeof(char)*128);
    char auxCompleta[1024];

	int tamBuf = sizeof buf;
	int fd;
	int leido;
	bool implemented=false;
    bool corrupt = true;
    client_packet respuestaRTSP;    

	memset(buf, 0, tamBuf);

    //Verificar si "lectura" corresponde a un paquete completo o no   
    //Primero me fijo si el paquete tiene el doble CRLF y si "lecturaCompleta" tiene algo. (si tiene algo y encontré doble CRLF es xq se terminó el paquete)

    //Final de mensaje compuesto o mensaje simple
     if( ( strstr(lectura, dobleCRLF)>0 && (!strlen(lecturaCompleta)) ) || (strstr(lectura, dobleCRLF) > 0) || (memcmp(lectura, "\r\n", 2) == 0)){

        //Concateno la ultima lectura del cliente con el viejo paquete
        lecturaCompleta = strncat(lecturaCompleta, lectura, strlen(lectura));
        //Ok, el paquete está finalmente completo
        respuestaRTSP.pckComplete = true;
        
        //=============================
            /*Parseo del método*/
        //=============================
        //Abro el archivo con los tipos de métodos RTSP implementados por el servidor 
        if ((fd = open ("config/metodos", O_RDONLY)) < 0)
        {
            perror("Fracaso en abrir el archivo de metodos, open dijo:");
            exit(1);
        }
        
        //Leo el archivo y comparo si el metodo escrito en el paquete es igual a alguno de los que están en el archivo de metodos
        while (( ( leido = read(fd, buf, 1)) > 0) && !implemented)
        {
            //recorre hasta encontrar un \n, cuando lo encuentra compara con el paquete y si es igual copia el valor del metodo a la estructura, establece implemented en true y sale
            if(memcmp(buf, "\n", 1)==0){
                if(memcmp(lecturaCompleta, token, strlen(token))==0 ){
                    memset(respuestaRTSP.method, 0, sizeof(respuestaRTSP.method));
                    memcpy(respuestaRTSP.method, token, strlen(token)+1 );
                    implemented = true;
                    memset(token, 0, sizeof token);
                }
                else
                    memset(token, 0, sizeof token);
            }
            else{
                memcpy(token, strcat(token, buf), 2); //el n acá era "1" antes
            }
        }

        //Si está implementado es porque el mensaje es correcto y ya está listo para ser ordenado (es completo)

        //=============================
            /*Parseo del paquete*/
        //=============================
        if( (strstr(lecturaCompleta, "RTSP/") != NULL) && (strstr(lecturaCompleta, "CSeq:") != NULL) )
            corrupt = false;
        else
            corrupt = true;

        if(implemented && !corrupt){

            //Esta es como la parte más larga de parseo, por ahora solo leo Cseq y escribo el apropiado en la estructura/respuesta
            //Delimitadores
            const char s[3] = " \n\r";
            //================================
                /*URI*/
            //================================
            strncpy(auxCompleta, lecturaCompleta, strlen(lecturaCompleta));
            pToken = strstr(auxCompleta,"rtsp:");
            aux = strtok(pToken, s);
            strncpy(respuestaRTSP.uri, aux, strlen(aux));

            //================================
                /*FileName from URI*/
            //================================
            //Ciertos clientes solo ponen el archivo solicitado en la URI a partir de DESCRIBE y no en OPTIONS y PLAY está puesto porque DESCRIBE no es necesario implementarlo
            if(memcmp(respuestaRTSP.method, "DESCRIBE", 8) == 0 || memcmp(respuestaRTSP.method, "PLAY", 4) == 0){
                strncpy(auxCompleta, lecturaCompleta, strlen(lecturaCompleta));

                //rtsp://192.168.0.102:8000/sample2.avi RTSP/1.0

                pToken = strstr(auxCompleta,"rtsp:");
                aux = strtok(pToken, "//");
                #ifdef DEBUG
                    write(STDOUT_FILENO, "\nAUX:", 5);
                    write(STDOUT_FILENO, aux, strlen(aux));
                #endif

                //acá hay que preguntar 8000/ existe?
                aux = strtok(NULL, "/");
                #ifdef DEBUG
                    write(STDOUT_FILENO, "\nAUX:", 5);
                    write(STDOUT_FILENO, aux, strlen(aux));
                #endif

                //acá hay que preguntar si hay algo después de / o solo un espacio
                aux = strtok(NULL, " ");
                #ifdef DEBUG
                    write(STDOUT_FILENO, "\nAUX:", 5);
                    write(STDOUT_FILENO, aux, strlen(aux));
                #endif

                memset(respuestaRTSP.fileToPlay, 0, sizeof(respuestaRTSP.fileToPlay));
                strncpy(respuestaRTSP.fileToPlay, aux, strlen(aux));
                #ifdef DEBUG
                    write(STDOUT_FILENO, "\nFile to play:", 14);
                    write(STDOUT_FILENO, respuestaRTSP.fileToPlay, strlen(respuestaRTSP.fileToPlay));
                    write(STDOUT_FILENO, "<-", 2);
                #endif
            }

            //================================
                /*RTSP Version*/
            //================================

            strncpy(auxCompleta, lecturaCompleta, strlen(lecturaCompleta));
            pToken = strstr(auxCompleta,"RTSP/");
            pToken = pToken + 5;
            aux = strtok(pToken, s);
            strncpy(respuestaRTSP.uri, aux, strlen(aux));

            //================================
                /*CSEQ*/
            //================================
            
            strncpy(auxCompleta, lecturaCompleta, strlen(lecturaCompleta));
            pToken = strstr(auxCompleta,"CSeq:");
            pToken = pToken + 5;
            aux = strtok(pToken, s);
            strncpy(respuestaRTSP.cseq, aux, strlen(aux));
        }
        //Sino, el metodo es: "no implementado" o corrupto
        else if(corrupt){
            memset(respuestaRTSP.method, 0, sizeof(respuestaRTSP.method));
            memcpy(respuestaRTSP.method, "Bad Request", strlen("Bad Request")+1 );
        }
        else if(!implemented)
        {   
            memset(respuestaRTSP.method, 0, sizeof(respuestaRTSP.method));
            memcpy(respuestaRTSP.method, "Not Implemented", strlen("Not Implemented")+1 );
        }

        //Cierro archivo "metodos.txt"
        close (fd);
     }
	 //Inicio o medio de un mensaje compuesto sin doble CRLF
	 else{
        //Primero guardo la lectura hecha hasta el momento en el puntero "lecturaCompleta"
        //Concateno la ultima lectura del cliente con el viejo paquete, técnicamente si "lecturaCompleta" está vació no debería generar problemas
        lecturaCompleta = strncat(lecturaCompleta, lectura, strlen(lectura));
        //Seteo el mensaje como incompleto
        memset(respuestaRTSP.method, 0, sizeof(respuestaRTSP.method));
	 	memcpy(respuestaRTSP.method, "Incomplete",11);
	 	respuestaRTSP.pckComplete = false;
        write(STDOUT_FILENO, "\nBuilding packet...", 19);
	 }
     pToken = NULL;
     aux = pToken;
     free(aux);
     free(pToken);
return respuestaRTSP;
}
