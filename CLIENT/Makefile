CC = g++
CFLAGS = -Wall 
DEPS = header.h 
OBJ = client.o 
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< -lssl -lcrypto -lpthread

client: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lssl -lcrypto -lpthread

clean:
	rm -rf *o client