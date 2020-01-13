#include <iostream>

#include "config.h"
#include "server_init.pb.h"
#include "../lib/udp.h"

using namespace std;

const char* SERVER_CTRL_IP="127.0.0.1";
const char* SERVER_NIC_IP="127.0.0.1";

int main(){
    udp uTest;
    uTest.addBind((char*)SERVER_CTRL_IP, SERVER_PORT_FOR_SERVER_INIT);
    uTest.addRemote((char*)CTRL_IP, CTRL_PORT_FOR_SERVER_INIT);
    packetIn packetInTest;
    packetOut packetOutTest;

    // Send server ip to controller
    packetInTest.set_serverctrlip((char*)SERVER_CTRL_IP);
    packetInTest.set_servernicip((char*)SERVER_NIC_IP);
    int size = packetInTest.ByteSize();
    void *buffer = malloc(size);
    packetInTest.SerializeToArray(buffer, size);
    uTest.send(buffer, size);

    // Receive assignment from controller
    uTest.receive();
    packetOutTest.ParseFromArray(uTest.recvBuffer, uTest.recvDataLen);
    cout<<"Connect to ToR "<<packetOutTest.torid()<<endl;
    cout<<"Random factor is "<<packetOutTest.randval()<<endl;

    return 0;
}
