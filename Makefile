CFLAGS=-g -Wall
TARGETS=server

all: $(TARGETS)

server: server.o atenderClienteTCP.o atenderClienteUDP.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lpthread

clean:
	rm -f $(TARGETS) *- *.o