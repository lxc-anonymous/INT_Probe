/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/

header ethernet_t {  
    bit<48>   dstAddr;
    bit<48>   srcAddr;
}

header type_1_t {
    bit<16>   type;    // 0x700:sr; 0x701:int
}

header sr_hdr_t {
    bit<8>    rsvd;
    bit<8>    sr_length;
}

header sr_t {   
    bit<8>    rsvd;
    bit<8>    next_port;
}

header type_2_t {   
    bit<16>   type;     // 0x701:int; 0x800:ipv4
}

header int_option_t {   // len = 1B
    bit<1>    type;     // 0 = int probe; 1 = specific flow
    bit<7>    rsvd;          
    bit<8>    int_num;         
}

header inthdr_t {   // len = 28B
    bit<8>    sw_id;
    bit<8>    ingress_port;
    bit<8>    egress_port;
    bit<8>    rsvd0;
    bit<48>   ingress_global_timestamp;
    bit<48>   egress_global_timestamp;
    bit<32>   enq_timestamp;
    bit<8>    rsvd1;
    bit<24>   enq_qdepth;
    bit<32>   deq_timedelta;
    bit<8>    rsvd2;
    bit<24>   deq_qdepth;
}

header sr_length_md_t {
    bit<8>    sr_length;
}

struct headers {
    ethernet_t      ethernet; 
    type_1_t[1]     type_1;
    sr_hdr_t[1]     sr_hdr;
    sr_t[16]        sr;
    type_2_t        type_2;
    int_option_t    int_option;
    inthdr_t        inthdr;
}

struct metadata {
    sr_length_md_t  sr_length_md; 
}

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser MyParser(packet_in packet,
                out headers hdr,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {

    state start {
        transition parse_ethernet;
    }

    state parse_ethernet {
        packet.extract(hdr.ethernet);
        transition parse_type_1;
    }

    state parse_type_1 {
        packet.extract(hdr.type_1[0]);
        transition parse_sr_hdr;
    }

    state parse_sr_hdr {
        packet.extract(hdr.sr_hdr[0]);
        meta.sr_length_md.sr_length = hdr.sr_hdr[0].sr_length;
        transition select(meta.sr_length_md.sr_length) {
            2: parse_type_2;
            default: parse_sr;
        }
    }

    state parse_sr {
        meta.sr_length_md.sr_length = meta.sr_length_md.sr_length - 2;
        packet.extract(hdr.sr.next);
        transition select(meta.sr_length_md.sr_length) {
            2: parse_type_2;
            default: parse_sr;
        }
    }

    state parse_type_2 {
        packet.extract(hdr.type_2);
        transition select(hdr.type_2.type) {
            0x701: parse_int_option;
            0x800: accept;
        }
    }

    state parse_int_option {
        packet.extract(hdr.int_option);
        transition accept;
    }
}

/*************************************************************************
************   C H E C K S U M    V E R I F I C A T I O N   *************
*************************************************************************/

control MyVerifyChecksum(inout headers hdr, inout metadata meta) {   
    apply {  }
}

/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {
    action drop() {
        mark_to_drop();
    }

    action do_sr() {
        standard_metadata.egress_spec = (bit<9>)hdr.sr[0].next_port;
        hdr.sr.pop_front(1);
        hdr.sr_hdr[0].sr_length = hdr.sr_hdr[0].sr_length-2;
    }

    action do_sr_final() {
        hdr.type_1.pop_front(1);
        hdr.sr_hdr.pop_front(1);
    }
    
    apply {
        do_sr();
        if(hdr.sr_hdr[0].sr_length==2) {
            do_sr_final();
        }
    }
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyEgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {  
    action drop() {
        mark_to_drop();
    }  

    action do_int(bit<8> sw_id) {
        hdr.inthdr.setValid();

        hdr.inthdr.sw_id = sw_id;
        hdr.inthdr.ingress_port=(bit<8>)standard_metadata.ingress_port;
        hdr.inthdr.egress_port=(bit<8>)standard_metadata.egress_port;
        hdr.inthdr.ingress_global_timestamp=(bit<48>)standard_metadata.ingress_global_timestamp;
        hdr.inthdr.egress_global_timestamp=(bit<48>)standard_metadata.egress_global_timestamp;
        hdr.inthdr.enq_timestamp=(bit<32>)standard_metadata.enq_timestamp;
        hdr.inthdr.enq_qdepth=(bit<24>)standard_metadata.enq_qdepth;
        hdr.inthdr.deq_timedelta=(bit<32>)standard_metadata.deq_timedelta;
        hdr.inthdr.deq_qdepth=(bit<24>)standard_metadata.deq_qdepth;

        hdr.int_option.int_num = hdr.int_option.int_num + 1;
    }

    table int_table {
        actions = {
            do_int;
            NoAction;
        }
        default_action = NoAction();
    }
    
    apply {

        if (hdr.int_option.isValid()) {
            int_table.apply();
        }
        else {
            NoAction();
        }
    }
}

/*************************************************************************
*************   C H E C K S U M    C O M P U T A T I O N   **************
*************************************************************************/

control MyComputeChecksum(inout headers hdr, inout metadata meta) {
    apply {}
}

/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control MyDeparser(packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.ethernet);
        packet.emit(hdr.type_1);
        packet.emit(hdr.sr_hdr);
        packet.emit(hdr.sr);
        packet.emit(hdr.type_2);
        packet.emit(hdr.int_option);
        packet.emit(hdr.inthdr);
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
MyParser(),
MyVerifyChecksum(),
MyIngress(),
MyEgress(),
MyComputeChecksum(),
MyDeparser()
) main;
