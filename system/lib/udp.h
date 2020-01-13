#ifndef _UDP_H_
#define _UDP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

using namespace std;

class udp
{
    public:
        udp();
        ~udp();
        void addBind(char* localIP, int localPort);
        void addRemote(char* remoteIP, int remotePort);
        void send(void* message, int messageLen);
        void receive();
    
        char recvBuffer[1024];
        int recvDataLen;    // Data received

    private:
        char* localIP;
        int localPort;
        char* remoteIP;
        int remotePort;

        int localSockFd;
        int localBindFd;
        struct sockaddr_in remoteSockAddr;
        struct sockaddr_in localSockAddr;
};

#endif
