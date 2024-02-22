//server
#include "defaults.h"
#include <winsock2.h>
#include <ws2tcpip.h> 
#include <stdio.h>
#include <cstring>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <map>
#include <vector>
            


std::mutex mtx;
std::condition_variable cv;


int echo( SOCKET* clientSock, const char* recvBuff, int len){

    if (send(*clientSock, recvBuff, len, 0) == SOCKET_ERROR){
        printf("\necho fail %d", WSAGetLastError());
        closesocket(*clientSock);
    return 1;
    }
    printf("\necho sent");

    return 0;
}

/// @brief extract the 10 char ID from recv buff
/// @param recvbuff the the buffer sent from client 
/// @return the first 10 elements from recvbuff
char* getID(char* recvbuff, bool senderId = true){
    char *sockID = new char[SOCK_ID_LEN];

    if (senderId){
        for (int i=0; i< SOCK_ID_LEN; i++){
            sockID[i] = recvbuff[i];
        }
        return sockID;
    }
    else{
        for (int i=0; i< SOCK_ID_LEN; i++){
            sockID[i] = recvbuff[i+SOCK_ID_LEN];
        }

        return sockID;
    }
}

/// @brief creates client socket and recieves data and echos recived message 
void handleClient(SOCKET* clientSock, char *recvbuf){
    int iResult;

    std::lock_guard<std::mutex> guard(mtx);

    do{
        iResult =  recv(*clientSock,recvbuf,(int) BUFF_LEN, 0);
        if (iResult > 0){
            printf("\nhandling client");
            //echo
            // echo(clientSock, (char *) MSG_SENT,MSG_SENT_LEN);
            echo(clientSock, recvbuf,iResult);
            printf("\necho finished?");
        
        }else if(iResult == 0){
            printf("\nConnection closing ...");
        }else{
            printf("\nHandle client Recv failed %d", WSAGetLastError());
            closesocket(*clientSock);
            delete clientSock;
            delete[] recvbuf;
        }
    }while( iResult>0);

    closesocket(*clientSock);

}

//next up logging


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

    //for client addressing  using 10 digit"phone numbers"
    std::vector <std::map<int, SOCKET>> client_addr;

    while(true){
    printf("\nServer running code %d", 100);

    char* recvbuff = new char[BUFF_LEN]{};
    SOCKET* clientSock = new SOCKET;
    *clientSock = accept(listenSock,NULL,NULL);

    if (*clientSock != INVALID_SOCKET){

        printf("\nsocket accepted");
        char* sockId = new char[SOCK_ID_LEN];

        std::thread cltHandth(handleClient, clientSock, recvbuff);
        cltHandth.detach();

        // const char *iD = getID(recvbuff);
        // strncpy(sockId, iD, SOCK_ID_LEN);
        // delete[] iD;
        //client_addr[sockId] = *clientSock; //client addressing

        delete[] sockId; 

    }else {
    printf("Accept failed: %d", WSAGetLastError());
    }
}

    closesocket(listenSock);
    WSACleanup();

    return 0;
}

