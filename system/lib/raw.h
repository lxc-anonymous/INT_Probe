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

#endif