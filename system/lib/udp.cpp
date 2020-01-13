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

#include "udp.h"

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


udp::udp()
{
    localSockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (localSockFd == -1)
    {
        cout << "Build socket fail." << endl;
        exit(-1);
    }
}

void udp::addBind(char* localIP, int localPort){

    localSockAddr.sin_family = AF_INET;
    localSockAddr.sin_port = htons(localPort);
    localSockAddr.sin_addr.s_addr = inet_addr(localIP);

    localBindFd = bind(localSockFd, (struct sockaddr *)&localSockAddr, sizeof(localSockAddr));
    if (localBindFd == -1)
    {
        cout << "Bind fail." << endl;
        exit(-1);
    }
    cout << "Bind success." << endl;

    listen(localSockFd, 30);
}

void udp::addRemote(char* remoteIP, int remotePort){
    remoteSockAddr.sin_family = AF_INET;
    remoteSockAddr.sin_port = htons(remotePort);
    remoteSockAddr.sin_addr.s_addr = inet_addr(remoteIP);
}

void udp::send(void* message, int messageLen)
{   
    socklen_t len = sizeof(remoteSockAddr);
    int a=sendto(localSockFd, message, messageLen, 0, (struct sockaddr*)&remoteSockAddr, len);
    // cout<<a<<endl;
}

void udp::receive()
{
    socklen_t len = sizeof(remoteSockAddr);
    memset(recvBuffer,0,sizeof(recvBuffer));
    recvDataLen = recvfrom(localSockFd, recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr*)&remoteSockAddr, &len);
    if (recvDataLen == 0){
        cout << " Receive fail." << endl;
        exit(-1);
    }
}

udp::~udp(void)
{
    // close(localSockFd);
    // close(localBindFd);
}

#endif
