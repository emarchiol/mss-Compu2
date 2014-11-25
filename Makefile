CFLAGS=-g -Wall
TARGETS=server

all: $(TARGETS)

server: server.o atenderClienteTCP.o atenderClienteUDP.o analizarRespuestaRTSP.o construirRespuestaRTSP.c
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lpthread

clear:
	rm -f $(TARGETS) *- *.o