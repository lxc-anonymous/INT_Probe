#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <thread>

#include "../third-party/redis-4.0.9/deps/hiredis/hiredis.h"
#include "../lib/udpserver.h"

using namespace std;

int main()
{
    redisContext *c4 = redisConnect("127.0.0.1", 6379);
    redisCommand(c4, "select 4");
    redisCommand(c4, "flushdb");

    redisCommand(c4, "rpush 5+1 4+160 0+129");
    redisCommand(c4, "rpush 5+2 4+168 1+137");
    redisCommand(c4, "rpush 5+3 4+176 2+145");
    redisCommand(c4, "rpush 5+4 4+184 3+153");
    redisCommand(c4, "rpush 6+1 4+160 0+130");
    redisCommand(c4, "rpush 6+2 4+168 1+138");
    redisCommand(c4, "rpush 6+3 4+176 2+146");
    redisCommand(c4, "rpush 6+4 4+184 3+154");
    redisCommand(c4, "rpush 7+1 4+160 0+131");
    redisCommand(c4, "rpush 7+2 4+168 1+139");
    redisCommand(c4, "rpush 7+3 4+176 2+147");
    redisCommand(c4, "rpush 7+4 4+184 3+155");


    redisContext *c5 = redisConnect("127.0.0.1", 6379);
    redisCommand(c5, "select 5");
    redisCommand(c5, "flushdb");

    redisCommand(c5, "rpush 4+1 5+161 0+128");
    redisCommand(c5, "rpush 4+2 5+169 1+136");
    redisCommand(c5, "rpush 4+3 5+177 2+144");
    redisCommand(c5, "rpush 4+4 5+185 3+152");
    redisCommand(c5, "rpush 6+1 5+161 0+130");
    redisCommand(c5, "rpush 6+2 5+169 1+138");
    redisCommand(c5, "rpush 6+3 5+177 2+146");
    redisCommand(c5, "rpush 6+4 5+185 3+154");
    redisCommand(c5, "rpush 7+1 5+161 0+131");
    redisCommand(c5, "rpush 7+2 5+169 1+139");
    redisCommand(c5, "rpush 7+3 5+177 2+147");
    redisCommand(c5, "rpush 7+4 5+185 3+155");


    redisContext *c6 = redisConnect("127.0.0.1", 6379);
    redisCommand(c6, "select 6");
    redisCommand(c6, "flushdb");
    
    redisCommand(c6, "rpush 4+1 6+162 0+128");
    redisCommand(c6, "rpush 4+2 6+170 1+136");
    redisCommand(c6, "rpush 4+3 6+178 2+144");
    redisCommand(c6, "rpush 4+4 6+186 3+152");
    redisCommand(c6, "rpush 5+1 6+162 0+129");
    redisCommand(c6, "rpush 5+2 6+170 1+137");
    redisCommand(c6, "rpush 5+3 6+178 2+145");
    redisCommand(c6, "rpush 5+4 6+186 3+153");
    redisCommand(c6, "rpush 7+1 6+162 0+131");
    redisCommand(c6, "rpush 7+2 6+170 1+139");
    redisCommand(c6, "rpush 7+3 6+178 2+147");
    redisCommand(c6, "rpush 7+4 6+186 3+155");


    redisContext *c7 = redisConnect("127.0.0.1", 6379);
    redisCommand(c7, "select 7");
    redisCommand(c7, "flushdb");
    
    redisCommand(c7, "rpush 4+1 7+163 0+128");
    redisCommand(c7, "rpush 4+2 7+171 1+136");
    redisCommand(c7, "rpush 4+3 7+179 2+144");
    redisCommand(c7, "rpush 4+4 7+187 3+152");
    redisCommand(c7, "rpush 5+1 7+163 0+129");
    redisCommand(c7, "rpush 5+2 7+171 1+137");
    redisCommand(c7, "rpush 5+3 7+179 2+145");
    redisCommand(c7, "rpush 5+4 7+187 3+153");
    redisCommand(c7, "rpush 6+1 7+163 0+130");
    redisCommand(c7, "rpush 6+2 7+171 1+138");
    redisCommand(c7, "rpush 6+3 7+179 2+146");
    redisCommand(c7, "rpush 6+4 7+187 3+154");


    redisContext *c2 = redisConnect("127.0.0.1", 6379);
    redisCommand(c2, "select 1");
    redisCommand(c2, "flushdb");

    redisCommand(c2, "rpush 4 16 10.0.0.1 00:00:00:00:00:00 17 10.0.0.2 00:00:00:00:00:00");
    redisCommand(c2, "rpush 5 18 10.0.0.3 00:00:00:00:00:00 19 10.0.0.4 00:00:00:00:00:00");
    redisCommand(c2, "rpush 6 24 10.0.0.5 00:00:00:00:00:00 25 10.0.0.6 00:00:00:00:00:00");
    redisCommand(c2, "rpush 7 26 10.0.0.7 00:00:00:00:00:00 27 10.0.0.8 00:00:00:00:00:00");

    return 0;
}
