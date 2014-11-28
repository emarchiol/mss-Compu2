CFLAGS = -g -Wall
TARGETS = server
TFLAG = -lpthread

debug: CC += -DDEBUG -g -Wall
debug: all

all: $(TARGETS)

server: server.o atenderClienteTCP.o atenderClienteUDP.o analizarRespuestaRTSP.o construirRespuestaRTSP.o
	$(CC) $(CFLAGS) -o $@ $^ $(TFLAG)

clear:
	rm -f $(TARGETS) *- *.o