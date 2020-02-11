CC = g++
CFLAGS = -Wall 
DEPS = header.h 
OBJ = server.o
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< -lssl -lcrypto -lpthread

server: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $< -lssl -lcrypto -lpthread

clean:
	rm -rf *o server