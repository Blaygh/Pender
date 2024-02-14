//server

#include <winsock2.h>
#include <ws2tcpip.h> 
#include <stdio.h>

#define DEFAULT_PORT "27015"
#define BUFF_LEN 512


int main(){
    
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0){
        printf("WSAStartup failed%d\n", WSAGetLastError);
    }

//GET ADDRESS INFO
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

    ZeroMemory(&hints, sizeof(hints)); // Initialize hints struct
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if(getaddrinfo(NULL,DEFAULT_PORT,&hints, &result) != 0){

        printf("Getaddrinfo failed%d\n", WSAGetLastError());
        WSACleanup();
    }
//CREATE A SOCKET
    SOCKET listenSock = INVALID_SOCKET;
    ptr = result;

    listenSock = socket(ptr->ai_family,ptr->ai_socktype,ptr->ai_protocol);
    if(listenSock == INVALID_SOCKET){
        printf("socket creation failed%d\n", WSAGetLastError());

        freeaddrinfo(result);
        WSACleanup();
    }


//BIND
    if(bind(listenSock, ptr->ai_addr,(int)ptr->ai_addrlen) == SOCKET_ERROR){
        printf("bind failed with error%d\n", WSAGetLastError());

        freeaddrinfo(result);
        closesocket(listenSock);
        WSACleanup();
    };

    freeaddrinfo(result);// addr info not needed after bind


    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR){
        printf( "Listen failed with error: %ld\n", WSAGetLastError() );
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    SOCKET ClientSocket1;
    SOCKET ClientSocket2;

    ClientSocket1 = INVALID_SOCKET;
    ClientSocket2 = INVALID_SOCKET;

// Accept a client socket
ClientSocket1 = accept(listenSock, NULL, NULL);
if (ClientSocket1 == INVALID_SOCKET) {
    printf("accept failed: %d\n", WSAGetLastError());
    closesocket(listenSock);
    WSACleanup();
    return 1;
}

ClientSocket2 = accept(listenSock, NULL, NULL);
if (ClientSocket1 == INVALID_SOCKET) {
    printf("accept failed: %d\n", WSAGetLastError());
    closesocket(listenSock);
    WSACleanup();
    return 1;
}


    char recvbuff[BUFF_LEN]={};
    int iResult1,iResult2, iSendResult;


    do{
        iResult1 = recv(ClientSocket1, recvbuff, (int)BUFF_LEN,0);
        if (iResult1>0){
            printf("Bytes received: %d\nClient sent: %s\n", iResult1, recvbuff);

            //echo
            iSendResult = send(ClientSocket2,recvbuff, iResult1,0);
            if (iSendResult == 0){
                printf("Echo failed %d\n", WSAGetLastError());
                closesocket(ClientSocket2);
                closesocket(ClientSocket1);
                WSACleanup();
            }
            printf("echo sent to client2\n");


            iResult2 = recv(ClientSocket2, recvbuff, (int)BUFF_LEN,0);
            if (iResult2>0){
                printf("Bytes received: %d\nClient sent: %s\n", iResult1, recvbuff);

                //echo
                iSendResult = send(ClientSocket1,recvbuff, iResult1,0);
                if (iSendResult == 0){
                    printf("Echo failed %d\n", WSAGetLastError());
                    closesocket(ClientSocket1);
                    WSACleanup();
                }
                printf("echo sent to client 1");


        }else if(iResult1 == 0){
            printf("\nClosing Connection\n");
        }else {
        printf("recv failed: %d\n ", WSAGetLastError());
        closesocket(ClientSocket1);
        closesocket(ClientSocket2);
        WSACleanup();
        return 1;
        }
    }
    } while(iResult1>0);

    //shutdown Send side of socket 

    iResult1 = shutdown(ClientSocket1,SD_SEND);
    if (iResult1 == SOCKET_ERROR){
        printf("Shutdown Error:%d ", WSAGetLastError());
        closesocket(ClientSocket1);
        WSACleanup();
    }

    closesocket(ClientSocket1);
    closesocket(ClientSocket2);
    WSACleanup();

    return 0;
}