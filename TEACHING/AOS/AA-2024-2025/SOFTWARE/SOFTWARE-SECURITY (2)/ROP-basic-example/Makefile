#/bin/bash

#makefile for a TCP client/server exploit with ROP 

all:
	gcc TCP-generic-server.c -fno-stack-protector -z execstack -o the-server
	gcc TCP-malicious-client.c helper.c -o the-attacker
