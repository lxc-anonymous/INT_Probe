import socket

from parse import *

# Receive INT probes
class Receive():

    # Build connection with NIC and the controller
    def __init__(self):
        self.sock_server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.server_addr = "/home/poi/Desktop/P4_INT_Ver5/tmp/tmp.sock"
        self.sock_server.bind(self.server_addr)

    def receive(self):
        self.sock_server.listen(5)
        conn, clintAddr=self.sock_server.accept()
        while True:
            data = conn.recv(2048)
            Parse(data).parse()



if __name__ == "__main__":
    receive_test = Receive()
    receive_test.receive()