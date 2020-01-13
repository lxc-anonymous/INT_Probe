#include <iostream>

#include "config.h"
#include "server_init.pb.h"
#include "../lib/udp.h"

using namespace std;

int main(){
    udp uTest;
    uTest.addBind((char*)CTRL_IP, CTRL_PORT_FOR_SERVER_INIT);

    packetIn packetInTest;
    packetOut packetOutTest;

    while (true){
        // Receive ip from server
        uTest.receive();
        packetInTest.ParseFromArray(uTest.recvBuffer, uTest.recvDataLen);
	cout<<"Receive request from "<<packetInTest.serverctrlip()<<endl;
        const char* SERVER_CTRL_IP=&packetInTest.serverctrlip()[0];
        uTest.addRemote((char*)SERVER_CTRL_IP, SERVER_PORT_FOR_SERVER_INIT);

        // Assign parameter for server
        packetOutTest.set_torid(1);
        packetOutTest.set_randval(7);
        int size = packetOutTest.ByteSize();
        void *buffer = malloc(size);
        packetOutTest.SerializeToArray(buffer, size);
        uTest.send(buffer, size);
    }   

    return 0;
}
