//client2

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <thread>
#include "defaults.h"

int recv_echo_msg( SOCKET* clientSock, char* recvBuff, int len){
    printf("\nrecieving echo");

    struct timeval tv;
    tv.tv_sec = TIMEOUT_IN_SECONDS; 
    tv.tv_usec = 0;  // Not using microseconds

    setsockopt(*clientSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    printf("\ntimeout set");


    int iRecv_res = recv(*clientSock, recvBuff, len, 0);
    printf("\n iRecv_res: %d",iRecv_res);

    printf("\n recvBuff: %s",recvBuff);

    if (iRecv_res>0){

        if (strcmp(recvBuff, MSG_SENT)) printf("\nMessage sent");
        else if(strcmp(recvBuff, MSG_DELVD)) printf("\nMessage delivered ");
        else printf("\nStatus code not recognized %s", recvBuff);

    }else if( iRecv_res == 0 ){
        printf("\nConnection Closing...");

    }else{
        printf("\necho fail %d", WSAGetLastError());
        closesocket(*clientSock);
        return 1;
    }

    tv.tv_sec = 0;  // No timeout
    tv.tv_usec = 0;  // Not using microseconds
    setsockopt(*clientSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    return 0;
}



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
    const char *sendBuff = "9732895371hello Client1 how you doing, -client 2 :)"; 
    char recvBuff[BUFF_LEN] = {};
    int iResult;



    int sendRes = send(connectSock,sendBuff,(int)strlen(sendBuff) + 1,0);

    if (sendRes == SOCKET_ERROR){
        printf("\nsend fail",WSAGetLastError());
        closesocket(connectSock);
    }

    printf("\nNumber of bytes sent: %d",sendRes);

    SOCKET *connectSock_ptr = &connectSock;
    std::thread echo_handle(recv_echo_msg, connectSock_ptr,recvBuff, MSG_SENT_LEN );
    echo_handle.join();



    //cleanup
    closesocket(connectSock);
    WSACleanup();


}

