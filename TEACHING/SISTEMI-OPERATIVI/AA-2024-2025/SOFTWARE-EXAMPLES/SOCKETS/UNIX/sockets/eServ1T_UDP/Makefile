SERVER	= server
CLIENT	= client

CSRC	= udpClient.c
SSRC	= udpServer.c

all: $(SERVER) $(CLIENT)

$(SERVER): 
	gcc -o $(SERVER).o $(SSRC) -c
	gcc -o $(SERVER) $(SERVER).o
$(CLIENT): 
	gcc -o $(CLIENT).o $(CSRC) -c
	gcc -o $(CLIENT) $(CLIENT).o
clean:
	rm -f *.o
	rm -f $(CLIENT)
	rm -f $(SERVER)
