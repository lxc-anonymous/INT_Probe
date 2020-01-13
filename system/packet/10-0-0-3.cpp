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
#include "../lib/raw.h"

using namespace std;

/********************************************************************************/
/********************************************************************************/

/********************************************************************************/
/********************************************************************************/

void sendInt(udp uTest_ctrl, raw rTest)
{
    // struct timeval tv;
    char buffer[1024];
    unsigned char dstMac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x77};
    unsigned char srcMac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x77};
    memcpy(buffer, dstMac, 6);
    memcpy(buffer + 6, srcMac, 6);
    while (true)
    {
        uTest_ctrl.receive();
        memcpy(buffer + 12, uTest_ctrl.buffer, uTest_ctrl.udpDataLen);
        rTest.send(buffer, uTest_ctrl.udpDataLen + 12);
    }
}

void receiveInt(udp uTest_ctrl, udp uTest_nic)
{
    while (true)
    {
        uTest_nic.receive();
        uTest_ctrl.send(uTest_nic.buffer, uTest_nic.udpDataLen);
    }
}

void route(udp uTest_nic, udp uTest_ctrl, int torId, int tor[])
{
    sleep(2);
    redisContext *c0 = redisConnect("127.0.0.1", 6379);
    redisContext *c1 = redisConnect("127.0.0.1", 6379);
    redisCommand(c1, "select 1");
    redisContext *c4 = redisConnect("127.0.0.1", 6379);
    redisCommand(c4, "select %d", torId);
    char buffer[1024];
    char buffer_1[1024];
    while (true)
    {
        // the same tor
        cout << "To this ToR (ToR " << torId << ")" << endl;
        unsigned char data[36] = {
            0x00, 0x01, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00,
            0x0a, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x04, 0x80, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        redisReply *r0 = (redisReply *)redisCommand(c1, "lrange %d 0 -1", torId);
        for (int i = 0; i < 2; i++)
        {
            memset(buffer_1 + 27, atoi(r0->element[3 * i]->str), 1);
            char *ip = r0->element[1 + 3 * i]->str;
            std::cout << "DstIP: " << ip << " " << atoi(r0->element[3 * i]->str) << endl;
            char *p;
            char *delim = (char *)".";
            p = strtok(ip, delim);
            for (int k = 0; k < 4; k++)
            {
                memset(buffer_1 + 8 + k, atoi(p), 1);
                p = strtok(NULL, delim);
            }
            uTest_nic.send(buffer_1, 36);
        }
        cout << endl;

        for (int i = 0; i < 1; i++) // ToR -> ToR
        {
            int temp[] = {1000000, 1000000};
            redisReply *path[2];
            redisReply *r = (redisReply *)redisCommand(c4, "keys %d+*", tor[i]);
            for (int j = 0; j < 4; j++) // All paths
            {
                redisReply *r1 = (redisReply *)redisCommand(c4, "lrange %s 0 -1", r->element[j]->str);
                redisReply *r2 = (redisReply *)redisCommand(c0, "get %s", r1->element[0]->str);
                redisReply *r3 = (redisReply *)redisCommand(c0, "get %s", r1->element[1]->str);

                int timeDelta = atoi(r2->str) + atoi(r3->str);
                if (timeDelta < temp[0])
                {
                    if (temp[0] < temp[1])
                    {
                        temp[1] = timeDelta;
                        path[1] = r1;
                    }
                    else
                    {
                        temp[0] = timeDelta;
                        path[0] = r1;
                    }
                }
                else if (timeDelta < temp[1])
                {
                    temp[1] = timeDelta;
                    path[1] = r1;
                }
            }

            // Create packets
            std::cout << "To ToR " << tor[i] << endl;

            unsigned char Path[12] = {0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            for (int j = 0; j < 2; j++) // Set paths in the packet
            {
                memset(Path + 3, atoi(path[j]->element[0]->str + 2), 1);
                memset(Path + 5, atoi(path[j]->element[1]->str + 2), 1);
                memcpy(buffer + 24 + 12 * j, Path, 12);
                std::cout << "Path " << j + 1 << ": " << atoi(path[j]->element[0]->str + 2) << " " << atoi(path[j]->element[1]->str + 2) << endl;
            }

            //********************************************************************************//
            //********************************************************************************//
            unsigned char cmdHdr[8] = {0x00, 0x02, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00};
            unsigned char key[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            for (int j = 0; j < 2; j++)
            {
                redisReply *r4 = (redisReply *)redisCommand(c1, "lrange %d 0 -1", tor[i]);

                for (int k = 0; k < 2; k++)
                {
                    memset(buffer + 24 + 12 * k + 6, 128, 1);
                    memset(buffer + 24 + 12 * k + 7, atoi(r4->element[0 + 3 * j]->str), 1);
                }

                // char *mac = r4->element[2 + 3 * j]->str;
                // std::cout << "DstMac: " << mac << endl;
                // char *p;
                // char *delim = (char *)":";
                // p = strtok(mac, delim);
                // for (int k = 0; k < 6; k++)
                // {
                //     memset(key + k, atoi(p), 1);
                //     p = strtok(NULL, delim);
                // }

                char *ip = r4->element[1 + 3 * j]->str;
                std::cout << "DstIP: " << ip << " " << atoi(r4->element[0 + 3 * j]->str) << endl;
                char *p;
                char *delim = (char *)".";
                p = strtok(ip, delim);
                for (int k = 0; k < 4; k++)
                {
                    memset(key + k, atoi(p), 1);
                    p = strtok(NULL, delim);
                }

                memcpy(buffer, cmdHdr, 8);
                memcpy(buffer + 8, key, 16);

                uTest_nic.addRemote((char *)"192.168.11.110", 4000);
                uTest_nic.send(buffer, 24 + 12 * 2);
            }
            cout << endl;
            //********************************************************************************//
            //********************************************************************************//
        }
        sleep(2);
    }
}

void sendSpec(udp uTest_nic)
{
    int a;
    char buffer[1024];
    uTest_nic.addRemote((char *)"192.168.11.110", 4000);

    unsigned char data1[] = {
        0x01, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char data2[] = {
        0x02, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    while (true)
    {
        char *ip = new char[15];
        cin >> a >> ip;
        if (a == 1)
        {
            cout << "Add specINT rule." << endl;
            char *delim = (char *)".";
            ip = strtok(ip, delim);
            for (int k = 0; k < 4; k++)
            {
                memset(data1 + 8, atoi(ip), 1);
                ip = strtok(NULL, delim);
            }
            memcpy(buffer, data1, 24);
            uTest_nic.send(buffer, 24);
        }
        else if (a == 2)
        {
            cout << "Del specINT rule." << endl;
            char *delim = (char *)".";
            ip = strtok(ip, delim);
            for (int k = 0; k < 4; k++)
            {
                memset(data2 + 8, atoi(ip), 1);
                ip = strtok(NULL, delim);
            }
            memcpy(buffer, data2, 24);
            uTest_nic.send(buffer, 24);
        }
    }
}

int main()
{
    udp uTest_ctrl;
    uTest_ctrl.addBind((char *)"192.168.1.201", 40000);
    uTest_ctrl.addRemote((char *)"192.168.1.220", 50000);

    udp uTest_nic;
    uTest_nic.addBind((char *)"192.168.11.101", 3000);

    raw rTest((char *)"eth17");

    redisContext *c = redisConnect("127.0.0.1", 6379);

    thread t1(sendInt, uTest_ctrl, rTest);
    thread t2(receiveInt, uTest_ctrl, uTest_nic);
    int tor[]={4, 6, 7};
    thread t3(route, uTest_nic, uTest_ctrl, 5, tor);
    thread t4(sendSpec, uTest_nic);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}
