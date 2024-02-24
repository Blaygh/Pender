//client2

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <thread>
#include "defaults.h"

int recv_echo_msg( SOCKET* clientSock, char* recvBuff, int len){

    struct timeval tv;
    tv.tv_sec = TIMEOUT_IN_SECONDS; 
    tv.tv_usec = 0;  // Not using microseconds

    setsockopt(*clientSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    int iRecv_res = recv(*clientSock, recvBuff, len, 0);

    if (iRecv_res>0){

        if (recvBuff == MSG_SENT) printf("Message sent");
        else if(recvBuff == MSG_SENT) printf("Message delivered ");

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

    SOCKET *connectSock_ptr = &connectSock;
    std::thread echo_handle(recv_echo_msg, connectSock_ptr,recvBuff, MSG_SENT_LEN );
    echo_handle.join();



// do {
//     iResult = recv(connectSock, recvBuff, BUFF_LEN, 0);
//     if (iResult > 0) {
//         printf("\nReceived echo: %s", recvBuff);

//     } else if (iResult == 0) {
//         printf("\nConnection closed");
//     } else {
//         printf("\nrecv failed: %d", WSAGetLastError());
//     }
// } while (iResult > 0);

    //cleanup
    closesocket(connectSock);
    WSACleanup();


}

