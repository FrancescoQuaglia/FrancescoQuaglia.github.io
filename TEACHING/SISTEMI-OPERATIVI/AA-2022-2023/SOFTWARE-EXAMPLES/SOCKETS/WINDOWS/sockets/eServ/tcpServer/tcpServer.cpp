#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 2345
#define BACKLOG 10

int __cdecl main(void){
	WSADATA wsaData;
	int iResult;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	struct sockaddr_in servaddr;
	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//iResult = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		fflush(stdout);
		return 1;
	}

	// Set the SOCKET for the server to listen for client connections.
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		fflush(stdout);
		WSACleanup();
		return 1;
	}

	printf("socket correctly created\n");
	fflush(stdout);

	// Setup the TCP listening socket
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(DEFAULT_PORT);

	iResult = bind(ListenSocket, (const sockaddr*)&servaddr, sizeof(servaddr));
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		fflush(stdout);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	printf("socket address correctly set\n");
	fflush(stdout);

	iResult = listen(ListenSocket, BACKLOG);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		fflush(stdout);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	printf("socket put to the listening state\n");
	fflush(stdout);

	getConnection:
	// Accept a client socket
	printf("-----------------------------\n");
	printf("waiting for client connection\n");
	fflush(stdout);

	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		fflush(stdout);
		goto getConnection;
	}

	printf("client connected\n");
	fflush(stdout);

	// No longer need server socket

	iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
	if (iResult >= DEFAULT_BUFLEN) iResult = DEFAULT_BUFLEN - 1;
	if (iResult > 0) {
		recvbuf[iResult] = '\0';
		printf("Bytes received: %d - content is: %s\n", iResult,recvbuf);
		fflush(stdout);
		// Echo the buffer back to the sender
		iSendResult = send(ClientSocket, recvbuf, iResult, 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			fflush(stdout);
			closesocket(ClientSocket);
			goto getConnection;
		}
			printf("Bytes sent: %d\n", iSendResult);
			fflush(stdout);
		}
		if (iResult == 0) {
			printf("Connection closing\n");
			fflush(stdout);
		}
		if (iResult < 0) {
			printf("recv failed with error: %d\n", WSAGetLastError());
			fflush(stdout);
		}

	closesocket(ClientSocket);
	goto getConnection;
}