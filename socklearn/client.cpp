//client1

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "defaults.h"
#include <thread>

//creat recv function for incoming msgs

int recv_echo_msg( SOCKET* clientSock, char* recvBuff, int len){

    struct timeval tv;
    tv.tv_sec = TIMEOUT_IN_SECONDS; 
    tv.tv_usec = 0;  // Not using microseconds

    setsockopt(*clientSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    int iRecv_res = recv(*clientSock, recvBuff, len, 0);

    if (iRecv_res>0){

        if (strcmp(recvBuff, MSG_SENT)) printf("\nMessage sent");
        else if(strcmp(recvBuff, MSG_DELVD)) printf("\nMessage delivered ");
        else printf("\nStatus code not recognized %s", recvBuff);

    }else if( iRecv_res == 0 ){
        printf("\nConnection Closing...");

    }else{
        printf("echo fail %d", WSAGetLastError());
        closesocket(*clientSock);
        return 1;
    }

    tv.tv_sec = 0;  // No timeout
    tv.tv_usec = 0;  // Not using microseconds
    setsockopt(*clientSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    return 0;
}


int main(){
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

    if(getaddrinfo("localhost",DEFAULT_PORT,&hints,&result) != 0){
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
    const char *sendbuf = "9732895430\0hello from the other side, -Client1 :)\0";
    char recvbuf[BUFF_LEN]={};
    int iResult;

    int sendRes = send(connectSocket,sendbuf,(int)strlen(sendbuf) + 1,0);

    if (sendRes == SOCKET_ERROR){
        printf("\nsend fail",WSAGetLastError());
        closesocket(connectSocket);
    }

    printf("\nNumber of bytes sent: %d",sendRes);

    //using multhithreading to to handle echo

    SOCKET *connectSock_ptr = &connectSocket;
    std::thread echo_handle(recv_echo_msg, connectSock_ptr,recvbuf, MSG_SENT_LEN );
    echo_handle.join();


    printf("\nEcho: message sent %s", recvbuf);

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

//client 2 9732895430
//client 1 9732895371
