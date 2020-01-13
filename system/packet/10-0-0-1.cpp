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

#include "raw.h"
#include "udp.h"

using namespace std;

/********************************************************************************/
/********************************************************************************/

/********************************************************************************/
/********************************************************************************/

int main()
{
    char sendBuffer[1024];
    static unsigned char srProtocol[] = {0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    int srProtocolLen = sizeof(srProtocol);
    memcpy(sendBuffer, srProtocol, srProtocolLen);
    // raw rTest((char *)"ens36");
    // rTest.send(sendBuffer, 40);
    udp uTest;
    uTest.addBind((char *)"127.0.0.1", 7777);
    uTest.addRemote((char *)"127.0.0.1", 8888);
    uTest.send(sendBuffer, 40);
}
