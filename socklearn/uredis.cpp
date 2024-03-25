#include "hiredis/hiredis.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <map>

using namespace std;

int main() {
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
        return 1;
    }
    printf("Connected to redis\n");
    

    string key = "9732895430";
    string value = "here bitch";

    for(int x = 0; x < 10, x++){
        
    }

    redisReply *reply = (redisReply*)redisCommand(c, "HSET reciepients 9732895430 %s", "SOCKET1");
    reply = (redisReply*)redisCommand(c, "HSET reciepients 9732895431 %s", "SOCKET2");
    reply = (redisReply*)redisCommand(c, "HSET reciepients 9732895432 %s", "SOCKET3");
    reply = (redisReply*)redisCommand(c, "HSET reciepients 9732895433 %s", "SOCKET4");

    reply = (redisReply*)redisCommand(c, "HGETALL reciepients");
    if (reply == NULL) {
        printf("Command failed: %s\n", c->errstr);
       
    }
    if (reply->type != REDIS_REPLY_ARRAY) {
        printf("Unexpected reply type: %d\n", reply->type);
        freeReplyObject(reply);

    }
    for (int i = 0; i < reply->elements; i += 2) {
        if (reply->element[i] == NULL || reply->element[i+1] == NULL) {
            printf("NULL field or value\n");
            continue;
        }
        printf("Field: %s, Value: %s\n", reply->element[i]->str, reply->element[i+1]->str);
    }

    freeReplyObject(reply);
    redisFree(c);

    return 0;
}