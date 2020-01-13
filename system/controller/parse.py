import struct
import socket
import redis

class Parse():
    def __init__(self, pkt):
        self.pkt=pkt
        self.r=redis.Redis(unix_socket_path='/var/run/redis/redis.sock', db=4)

    def parse(self):
        print(len(self.pkt))
        [int_option, self.pkt] = struct.unpack("!14s2s%ds"%(len(self.pkt)-16), self.pkt)[1:]
        int_num = self.parse_int_option(int_option)[1]

        # Remove the first INT header
        self.pkt = struct.unpack("!32s%ds"%(len(self.pkt)-32), self.pkt)[1]
        for i in range(int_num - 1):
            [inthdr, self.pkt] = struct.unpack("!32s%ds"%(len(self.pkt)-32), self.pkt)
            [sw_id, egress_port, deq_timedelta] = self.parse_int(inthdr)
            print(sw_id, egress_port, deq_timedelta)
            self.r.lset(sw_id, 4*egress_port, deq_timedelta)

    # B: unsigned char 1B; H: unsigned short 2B; I: unsigned int 4B
    def parse_int_option(self, pkt):
        int_option = struct.unpack("!BB", pkt)
        int_type = (int_option[0] & 0x80) >> 7  # 1
        int_num = int_option[1]  # 2
        return int_type, int_num

    def parse_int(self, pkt):
        inthdr = struct.unpack("!BBBsIHIHIsHBIsHB", pkt)
        sw_id = inthdr[0]
        ingress_port = inthdr[1]
        egress_port = inthdr[2]
        ingress_global_timestamp = (inthdr[4]<<16)+inthdr[5]
        egress_global_timestamp = (inthdr[6]<<16)+inthdr[7]
        enq_timestamp = inthdr[8]
        enq_qdepth = (inthdr[10]<<8)+inthdr[11] 
        deq_timedelta = inthdr[12]
        deq_qdepth = (inthdr[14]<<8)+inthdr[15] 
        # return sw_id, ingress_port, egress_port, ingress_global_timestamp, egress_global_timestamp,\
        #         enq_timestamp, enq_qdepth, deq_timedelta, deq_qdepth
        return sw_id, egress_port, deq_timedelta


if __name__ == "__main__":
    pass
