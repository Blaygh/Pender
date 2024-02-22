//client2

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "defaults.h"


int main(){
    //initiate WSASTARTUP
    WSADATA wsadata;

    if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0){
        printf("WSAStartup failed %d", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    //addrinfo struct
    struct addrinfo *result,
                    *ptr,
                    hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;


    if (getaddrinfo("localhost", DEFAULT_PORT, &hints, &result) != 0){
        printf("getaddrinfo failed %d\n", WSAGetLastError());
        WSACleanup();
    }
    ptr = result;

    //create socket 
    SOCKET connectSock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (connectSock == INVALID_SOCKET){
        printf("Socket creation fail %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    //Connect 
    if (connect(connectSock,ptr->ai_addr,(int)ptr->ai_addrlen) == SOCKET_ERROR){
        printf("Socket connection fail %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    //BUFF initializations
    const char *sendBuff = "hello Client1 how you doing, -client 2 :)"; 
    char recvBuff[BUFF_LEN] = {};
    int iResult;



    int sendRes = send(connectSock,sendBuff,(int)strlen(sendBuff) + 1,0);

    if (sendRes == SOCKET_ERROR){
        printf("\nsend fail",WSAGetLastError());
        closesocket(connectSock);
    }

    printf("\nNumber of bytes sent: %d",sendRes);


do {
    iResult = recv(connectSock, recvBuff, BUFF_LEN, 0);
    if (iResult > 0) {
        printf("\nReceived echo: %s", recvBuff);

    } else if (iResult == 0) {
        printf("\nConnection closed");
    } else {
        printf("\nrecv failed: %d", WSAGetLastError());
    }
} while (iResult > 0);



    //recieve
    // int iSendbytes = recv(connectSock, recvBuff, (int)strlen(sendBuff),0);

    // if (iSendbytes == SOCKET_ERROR){
    //     printf("send fail",WSAGetLastError());
    //     closesocket(connectSock);
    // }
    // printf("%s", recvBuff);

    // //send 
    // iSendbytes = send(connectSock, sendBuff, (int)strlen(sendBuff)+1,0);

    // if (iSendbytes == SOCKET_ERROR){
    //     printf("send fail %d",WSAGetLastError());
    //     closesocket(connectSock); 
    // }

    //     iSendbytes = shutdown(connectSock, SD_SEND);
    // if (iSendbytes == SOCKET_ERROR) {
    //     printf("shutdown failed: %d\n", WSAGetLastError());
    //     closesocket(connectSock);
    //     WSACleanup();
    //     return 1;
    // }

        // Receive until the peer closes the connection
    // do {

    //     iSendbytes = recv(connectSock, recvBuff, (int)BUFF_LEN, 0);
    //     if ( iSendbytes > 0 )
    //         printf("Bytes received: %d\n", iSendbytes);
    //     else if ( iSendbytes == 0 )
    //         printf("Connection closed\n");
    //     else
    //         printf("recv failed: %d\n", WSAGetLastError());

    // } while( iSendbytes > 0 );


    //cleanup
    closesocket(connectSock);
    WSACleanup();


}

