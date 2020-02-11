CC = g++
CFLAGS = -Wall 
DEPS = header.h 
OBJ = 2018201103_assign_1_client.o 
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< -lssl -lcrypto -lpthread

client: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lssl -lcrypto -lpthread

clean:
	rm -rf *o client