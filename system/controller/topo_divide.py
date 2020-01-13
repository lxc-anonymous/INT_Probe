import redis
import random
import time

node_list = [2,2,2,2,2]
spine_num = node_list[0]
set_num = node_list[1]
leaf_num = node_list[1]
tor_num = node_list[2]
h_num = node_list[3]
pod_num = node_list[4]


def make_divide(l1_sw_id, l2_sw_id, r0, r1):

    l2_sw_list = map(int, r0.lrange(l1_sw_id, 0, -1)[2:-1:4])
    l1_sw_list = map(int, r1.lrange(l2_sw_id, 0, -1)[2:-1:4])

    l2_sw_list_divide = []
    l1_sw_list_divide = []
    for i in range(0, len(l1_sw_list), 2):
        l1_sw_list_divide.append(l1_sw_list[i:i+2])
    for i in range(0, len(l2_sw_list), 2):
        l2_sw_list_divide.append(l2_sw_list[i:i+2])

    return l1_sw_list_divide, l2_sw_list_divide


def get_port(l1_sw_list_divide, l2_sw_list_divide, r0, r1):
    # spine_list=[0,1,2,3], leaf_list=[100,110,120,130]
    sw_port_dict = {}
    src_sw_id = l2_sw_list_divide[0]

    for i in l1_sw_list_divide:
        sw_port_dict[i] = {}
        port_list = r0.lrange(i, 0, -1)

        port_num = (len(port_list)-1)/3
        for j in range(port_num):
            if int(port_list[4*j+2]) in l2_sw_list_divide:
                sw_port_dict[i][int(port_list[4*j+1][4:])] = [int(port_list[4*j+2]),int(port_list[4*j+3][4:])]

    for i in l2_sw_list_divide:
        sw_port_dict[i] = {}
        port_list = r1.lrange(i, 0, -1)

        port_num = (len(port_list)-1)/3
        for j in range(port_num):
            if int(port_list[4*j+2]) in l1_sw_list_divide:
                sw_port_dict[i][int(port_list[4*j+1][4:])] = [int(port_list[4*j+2]),int(port_list[4*j+3][4:])]

    return sw_port_dict, src_sw_id


def get_route(sw_port_dict, src_sw_id):
    hop_num = sum([len(sw_port_dict[i]) for i in sw_port_dict.keys()])/2
    route_list = []
    route_list_reverse=[]

    last_sw_id = src_sw_id
    while(hop_num != 1):
        sw_src = last_sw_id
        egress_port = sw_port_dict[sw_src].keys()[0]
        sw_dst = sw_port_dict[sw_src][egress_port][0]
        egress_port_reverse=sw_port_dict[sw_src][egress_port][1]

        if sw_dst == src_sw_id and len(sw_port_dict[src_sw_id].keys()) == 1:
            egress_port = sw_port_dict[sw_src].keys()[1]

        route_list.append(egress_port)
        route_list_reverse.append(egress_port_reverse)
        sw_dst = sw_port_dict[sw_src][egress_port][0]
        del(sw_port_dict[sw_src][egress_port])
        del(sw_port_dict[sw_dst][egress_port_reverse])
        last_sw_id = sw_dst
        hop_num -= 1

    # last hop
    sw_src = last_sw_id
    egress_port = sw_port_dict[sw_src].keys()[0]
    route_list.append(egress_port)
    route_list_reverse.append(sw_port_dict[sw_src][egress_port][1])
    route_list_reverse.reverse()
    # print(route_list,route_list_reverse)
    return route_list, route_list_reverse


if __name__ == "__main__":
    r0 = redis.Redis(unix_socket_path='/var/run/redis/redis.sock', db=0)
    r1 = redis.Redis(unix_socket_path='/var/run/redis/redis.sock', db=1)
    r2 = redis.Redis(unix_socket_path='/var/run/redis/redis.sock', db=2)
    r3 = redis.Redis(unix_socket_path='/var/run/redis/redis.sock', db=3)

    for i in range(set_num):
        l1_sw_id = spine_num*i
        l2_sw_id = spine_num*set_num+i
        
        [l1_sw_list_divide, l2_sw_list_divide] = make_divide(
            l1_sw_id, l2_sw_id, r0, r1)
        for m in l1_sw_list_divide:
            for n in l2_sw_list_divide:
                [sw_port_dict, src_sw_id] = get_port(m, n, r0, r1)
                [route_list, route_list_reverse]=get_route(sw_port_dict, src_sw_id)

                last_hop_port=int(r2.lindex(src_sw_id,1)[4:])
                last_hop_sw_id=r2.lindex(src_sw_id,2)
                first_hop_port=None
                for j in range((len(r3.lrange(last_hop_sw_id,0,-1))-1)/3):
                    if int(r3.lindex(last_hop_sw_id, 4*j+2))==src_sw_id:
                        first_hop_port=int(r3.lindex(last_hop_sw_id, 4*j+1)[4:])
                        break
                route_list=[first_hop_port]+route_list+[last_hop_port]
                route_list_reverse=[first_hop_port]+route_list_reverse+[last_hop_port]
                print(last_hop_sw_id)
                print(route_list)
                print(route_list_reverse)

                

    for i in range(pod_num):
        l1_sw_id = spine_num*set_num+i*leaf_num
        l2_sw_id = spine_num*set_num+pod_num*leaf_num+i*tor_num

        [l1_sw_list_divide, l2_sw_list_divide] = make_divide(
            l1_sw_id, l2_sw_id, r2, r3)
        for m in l1_sw_list_divide:
            for n in l2_sw_list_divide:
                [sw_port_dict, src_sw_id] = get_port(m, n, r2, r3)
                [route_list, route_list_reverse]=get_route(sw_port_dict, src_sw_id)
                print(src_sw_id)
                print(route_list)
                print(route_list_reverse)
