///server
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

#include <iostream>


std::mutex mtx;
std::condition_variable cv;


int echo( SOCKET* clientSock, const char* sendBuff, int len){

    if (send(*clientSock, sendBuff, len, 0) == SOCKET_ERROR){
        printf("\necho fail %d", WSAGetLastError());
        closesocket(*clientSock);
    return 1;
    }
    printf("\necho sent");

    return 0;
}

/// @brief extract the 10 char ID from recv buff
/// @param recvbuff the the buffer sent from client 
/// @param senderId default is true, returns sender ID, set to false for reciever ID
/// @return the first 10 elements from recvbuff
char* getID(char* recvbuff, bool senderId = true){
    char* sockID = new char[SOCK_ID_LEN]{};

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
void handleClient(SOCKET* clientSock, char *recvbuf, std::map<int,SOCKET> &sockaddr_map){
    int iResult;

    std::lock_guard<std::mutex> guard(mtx);
    char client_addr[11] = {};

    do{
        iResult =  recv(*clientSock,recvbuf,(int) BUFF_LEN, 0);
        if (iResult > 0){
            printf("\nhandling client");
            //echo
            echo(clientSock, (char *) MSG_SENT,MSG_SENT_LEN);
            //echo(clientSock, recvbuf,iResult);
            printf("\necho finished?");

            break;
        
        }else if(iResult == 0){
            printf("\nConnection closing ...");
        }else{
            printf("\nHandle client Recv failed %d", WSAGetLastError());
            closesocket(*clientSock);
            delete clientSock;
            delete[] recvbuf;
        }
    }while( iResult>0);

    char* id = getID(recvbuf);
    strncpy(client_addr,id, 10);
    client_addr[10] = '\0';

    printf("\nrecieved from %s", client_addr);

    //delete id from get id funct
    delete[] id; 


    // *sockadddr_map[clieint_addr];



    closesocket(*clientSock);

}

//next up sending messages

int send_msg(){
    
}


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
    std::map<int, SOCKET> client_addr_map;

    while(true){
    printf("\nServer running code %d", 100);

    char* recvbuff = new char[BUFF_LEN]{};
    SOCKET* clientSock = new SOCKET;
    *clientSock = accept(listenSock,NULL,NULL);

    if (*clientSock != INVALID_SOCKET){

        printf("\nsocket accepted");
        char* sockId = new char[SOCK_ID_LEN];

        std::thread cltHandth(handleClient, clientSock, recvbuff,std::ref(client_addr_map));
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


//implement addressing in sendbuff and recieve biff in both clients not done
//retrieve 10digit addressing

