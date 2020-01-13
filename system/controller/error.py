import nnpy
import struct
import datetime

sub = nnpy.Socket(nnpy.AF_SP, nnpy.SUB)
sub.connect("ipc:///tmp/bm-4-log.ipc")
sub.setsockopt(nnpy.SUB, nnpy.SUB_SUBSCRIBE, "")

while True:
    msg = sub.recv()
    print(len(msg))
    print("t5=%s"%datetime.datetime.now())
    msg = struct.unpack("<4sIII16sII4s", msg)
    sub_topic = msg[0]
    switch_id = msg[1]+(msg[2] << 8)
    num_statuses = msg[3]
    padding = msg[4]
    port = msg[5]
    status = msg[6]
    print(msg)
    # if status == 0:
        # print("switch %d -- port %d -- disconnect" % (switch_id, port))
        # message = "&%d+%d&" % (switch_id, port)
        # message_list_to_global_controller.append(message)
        # message_list_to_host.append(message)
