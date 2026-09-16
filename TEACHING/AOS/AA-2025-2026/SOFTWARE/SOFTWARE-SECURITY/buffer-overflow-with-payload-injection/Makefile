#/bin/bash

#makefile for a TCP client/server exploit with shell command injection

all:
	gcc TCP-echo-server.c -fno-stack-protector -z execstack -o the-server
	gcc TCP-malicious-client.c helper.c -o the-attacker

variant:
	gcc TCP-echo-server.c -fno-stack-protector -z execstack -o the-server
	gcc TCP-malicious-client.c helper.c -DVARIANT -o the-attacker
