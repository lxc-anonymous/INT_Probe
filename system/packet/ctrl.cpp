#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <hiredis/hiredis.h>
#include <sys/time.h>

#include "../lib/udp.h"

using namespace std;

/********************************************************************************/
/********************************************************************************/
// static unsigned char cmd[] = {0x00, 0x00};
//int cmdLen = sizeof(cmd);
static unsigned char srProtocol[] = {0x07, 0x00};
int srProtocolLen = sizeof(srProtocol);
static unsigned char intOption[] = {0x07, 0x01, 0x00, 0x00};
int intOptionLen = sizeof(intOption);
/********************************************************************************/
/********************************************************************************/

void sendIntPath(udp uTest, char* dstIp, unsigned char path[4][12])
{
    // struct timeval tv;
    uTest.addRemote(dstIp, 40000);
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, 1024);
        memcpy(buffer , srProtocol, srProtocolLen);
        for (int i = 0; i < 4; i++)
        {
            memcpy(buffer  + srProtocolLen, path[i], sizeof(path[i]));
            memcpy(buffer  + srProtocolLen + sizeof(path[i]), intOption, intOptionLen);
            uTest.send(buffer,  srProtocolLen + sizeof(path[i]) + intOptionLen);
            // gettimeofday(&tv,NULL);
            // printf("microsecond: %ld us\n",tv.tv_sec*1000000 + tv.tv_usec);  //微秒 
        }
        sleep(1);
    }
}

void receiveInt(udp uTest, redisContext *c)
{
    // struct timeval tv;
    redisReply *reply;
    while (true)
    {
        uTest.receive();
        // gettimeofday(&tv,NULL);
        // printf("microsecond: %ld us\n",tv.tv_sec*1000000 + tv.tv_usec);  //微秒 
        int INTNum = (int)((unsigned char)uTest.buffer[1]);
        for (int i = 0; i < INTNum; i++)
        {
            int swID = (int)((unsigned char)uTest.buffer[3 + 32 * i]);
            int egressPort = (int)((unsigned char)uTest.buffer[7 + 32 * i]);
            long int ingressGlobalTstamp = ((long int)((unsigned char)uTest.buffer[8 + 32 * i] << 40)) + ((long int)((unsigned char)uTest.buffer[9 + 32 * i] << 32)) +
                                           ((long int)((unsigned char)uTest.buffer[10 + 32 * i] << 24)) + ((long int)((unsigned char)uTest.buffer[11 + 32 * i] << 16)) +
                                           ((long int)((unsigned char)uTest.buffer[12 + 32 * i] << 8)) + ((long int)((unsigned char)uTest.buffer[13 + 32 * i]));
            long int egressGlobalTstamp = ((long int)((unsigned char)uTest.buffer[14 + 32 * i] << 40)) + ((long int)((unsigned char)uTest.buffer[15 + 32 * i] << 32)) +
                                          ((long int)((unsigned char)uTest.buffer[16 + 32 * i] << 24)) + ((long int)((unsigned char)uTest.buffer[17 + 32 * i] << 16)) +
                                          ((long int)((unsigned char)uTest.buffer[18 + 32 * i] << 8)) + ((long int)((unsigned char)uTest.buffer[19 + 32 * i]));
            int timeDelta = egressGlobalTstamp - ingressGlobalTstamp;
            cout << swID << " " << egressPort << " " << timeDelta << "  ";
            redisAppendCommand(c, "set %d+%d %d", swID, egressPort, timeDelta);
        }
        redisGetReply(c,(void**)reply);
        // freeReplyObject(reply);
        // gettimeofday(&tv,NULL);
        // printf("microsecond: %ld us\n",tv.tv_sec*1000000 + tv.tv_usec);  //微秒 
        cout << endl;
    }
}


int main()
{
    redisContext *c = redisConnect("127.0.0.1", 6379);

    udp uTest1;
    uTest1.addBind((char *)"192.168.1.220", 50000);

    unsigned char path1[4][12] = {
        {0x00, 0x0c, 0x00, 0xa0, 0x00, 0x81, 0x00, 0xa9, 0x00, 0x88, 0x80, 0x10},
        {0x00, 0x0c, 0x00, 0xb0, 0x00, 0x91, 0x00, 0xb9, 0x00, 0x98, 0x80, 0x10},
        {0x00, 0x0c, 0x00, 0xa8, 0x00, 0x89, 0x00, 0xa1, 0x00, 0x80, 0x80, 0x10},
        {0x00, 0x0c, 0x00, 0xb8, 0x00, 0x99, 0x00, 0xb1, 0x00, 0x90, 0x80, 0x10}};

    // udp uTest2((char *)"10.112.49.103", 50001, (char *)"10.112.165.85", 40001);
    unsigned char path2[4][12] = {
        {0x00, 0x0c, 0x00, 0xa2, 0x00, 0x83, 0x00, 0xab, 0x00, 0x8a, 0x80, 0x18},
        {0x00, 0x0c, 0x00, 0xb2, 0x00, 0x93, 0x00, 0xbb, 0x00, 0x9a, 0x80, 0x18},
        {0x00, 0x0c, 0x00, 0xaa, 0x00, 0x8b, 0x00, 0xa3, 0x00, 0x82, 0x80, 0x18},
        {0x00, 0x0c, 0x00, 0xba, 0x00, 0x9b, 0x00, 0xb3, 0x00, 0x92, 0x80, 0x18}};

    
    thread t1(receiveInt, uTest1, c);
    thread t2(sendIntPath, uTest1, (char *)"192.168.1.200", path1);
    thread t3(sendIntPath, uTest1, (char *)"192.168.1.202", path2);
    // thread t3(sendIntPath, uTest2, path2);
    // thread t4(receiveInt, uTest2, c);
    t1.join();
    t2.join();
    t3.join();
    // t3.join();

    return 0;
}