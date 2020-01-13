import socket
import struct


# Receive INT probes
class Receive():

    # Build connection with NIC and the controller
    def __init__(self):
        self.sock_nic = socket.socket(socket.PF_PACKET, socket.SOCK_RAW, socket.htons(0x0003))
        self.sock_ctrl = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.server_addr = "/home/poi/Desktop/P4_INT_Ver5/tmp/tmp.sock"
        self.sock_ctrl.connect(self.server_addr)

    def receive(self):
        while True:
            data = self.sock_nic.recv(2048)
            if not data:
                print ("Client has exist")
                break
            if len(data)>32:
                self.update(data)
            
        self.sock_nic.close()
        self.sock_ctrl.close()


    def update(self, data):
        self.sock_ctrl.sendall(data)


if __name__ == "__main__":
    receive_test = Receive()
    receive_test.receive()