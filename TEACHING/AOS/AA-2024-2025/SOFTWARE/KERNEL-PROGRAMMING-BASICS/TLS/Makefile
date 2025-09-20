
all:
	gcc main.c ./lib/tls.c -Xlinker --wrap=pthread_create -lpthread -DTLS -o test -I./include
