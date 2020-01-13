#!/usr/bin/env python
import argparse
import sys
import socket
import random
import struct
import time

from scapy.all import sendp, send, get_if_list, get_if_hwaddr, get_if_addr
from scapy.all import Packet
from scapy.all import Ether, IP, UDP, TCP

def get_if():
    ifs=get_if_list()
    iface=None # "h1-eth0"
    for i in get_if_list():
        if "eth0" in i:
            iface=i
            break;
    if not iface:
        print "Cannot find eth0 interface"
        exit(1)
    return iface

def main():

    iface = get_if()

    pkt =  Ether(src=get_if_hwaddr(iface), dst='00:00:00:02:02:02', type=1792)
    pkt = pkt / '\x00\x04' / '\x00\x03'
    pkt = pkt / '\x07\x01' / '\x00\x00'
    
    for i in xrange(5):
        sendp(pkt, iface=iface, verbose=False)
        pkt.show2()
        time.sleep(1)
        


if __name__ == '__main__':
    main()
