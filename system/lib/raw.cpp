#ifndef _RAW_H_
#define _RAW_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <bits/ioctls.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <iostream>
#include <errno.h>

#include "raw.h"

using namespace std;

class raw
{
    public:
        raw(char* interface);
        ~raw();
        void send(char sendBuffer[], int sendPktLen);
        void receive();

    protected:
        char recvBuffer[1024];
        int recvPktLen;

    private:
        int localSockFd;
        struct sockaddr_ll device;
};

raw::raw(char* interface)
{   
    char srcMac[6];	
    localSockFd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", interface);
    ioctl(localSockFd, SIOCGIFHWADDR, &ifr);
    close(localSockFd);
    device.sll_ifindex = if_nametoindex(interface);
    device.sll_family = AF_PACKET;
    memcpy(device.sll_addr, srcMac, 6);
    device.sll_halen = htons(6);
    localSockFd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
}

void raw::send(char sendBuffer[], int sendPktLen)
{
    sendto(localSockFd, sendBuffer, sendPktLen, 0, (struct sockaddr *)&device, sizeof(device));
}

void raw::receive()
{
    socklen_t len=sizeof(device);
    memset(recvBuffer,0,sizeof(recvBuffer));
    recvPktLen=recvfrom(localSockFd, recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr *)&device, &len);
}

raw::~raw(void)
{
}

#endif
