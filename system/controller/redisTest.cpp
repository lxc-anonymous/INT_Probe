#include <stdio.h>
#include <hiredis/hiredis.h>

int main()
{   
    // int nodeList[5]={10,10,10,10,10};
    // int spine_num=nodeList[0];
    // int leaf_num=nodeList[1];
    // int tor_num=nodeList[2];
    // int set_num=nodeList[3];
    // int pod_num=nodeList[4];
    redisContext* conn=redisConnectUnix("/var/run/redis/redis.sock");
    if(conn->err){
        printf("connection error:%s\n",conn->errstr);
    }
  
    // redisReply* reply = (redisReply*)redisCommand(conn,"set foo 1234");
    // freeReplyObject(reply);

    redisReply* reply = (redisReply*)redisCommand(conn,"lrange 0 0 -1");
    printf("%s\n",reply->str);
    freeReplyObject(reply);

    redisFree(conn);
    return 0;
}

