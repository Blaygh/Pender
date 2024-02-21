//client1

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "defaults.h"
#include <thread>




int main(int argc, char *argv[]){
    WSADATA wsadata;

    if (WSAStartup(MAKEWORD(2,2), &wsadata)!=0){
        printf("Couldnt initiat WSAStartup%d\n", GetLastError);
        WSACleanup();
        return 1;
    }

    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;


    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if(getaddrinfo(argv[1],DEFAULT_PORT,&hints,&result) != 0){
        printf("\ngetaddrinfo failed%d");
        WSACleanup();
        return 1;
    }

    SOCKET connectSocket = INVALID_SOCKET;

    ptr = result;

    connectSocket = socket(result->ai_family,result->ai_socktype,result->ai_protocol);

    if(connectSocket == INVALID_SOCKET){
        printf("\nsocket creation failed%d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    if (connect(connectSocket,ptr->ai_addr,(int)ptr->ai_addrlen) == SOCKET_ERROR){
        printf("\nConnection failed%d", WSAGetLastError());

        //LOOK INTO TRYING NEXT ADDRESS RETURNED BY getaddrinfo 
        //instead of freeing the resources

        closesocket(connectSocket);
        connectSocket = INVALID_SOCKET;
        freeaddrinfo(result);
        return 1;
    }


    if (connectSocket == INVALID_SOCKET) {
        printf("\nUnable to connect to server!%d");
        WSACleanup();
        return 1;
}

    //sending buffer
    const char *sendbuf = "hello from the other side, -Client1 :)\0";
    char recvbuff[BUFF_LEN];

    int sendRes = send(connectSocket,sendbuf,(int)strlen(sendbuf) + 1,0);

    if (sendRes == SOCKET_ERROR){
        printf("\nsend fail",WSAGetLastError());
        closesocket(connectSocket);
    }

    printf("\nNumber of bytes sent: %d",sendRes);

    char recvbuf[BUFF_LEN];
    int iResult;

do {
    iResult = recv(connectSocket, recvbuf, BUFF_LEN, 0);
    if (iResult > 0) {
        printf("\nReceived echo: %s", recvbuf);

    } else if (iResult == 0) {
        printf("\nConnection closed");
    } else {
        printf("\nrecv failed: %d", WSAGetLastError());
    }
} while (iResult > 0);

 
    closesocket(connectSocket);
    WSACleanup();
    return 0;
}

//initiate WSASTARTUP
//addrinfo struct
//create socket 
//Connect 
//send 
//recieve
//cleanup
