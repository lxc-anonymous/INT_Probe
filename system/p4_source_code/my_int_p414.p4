/* Copyright 2013-present Barefoot Networks, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/********************************************************************************/
/********************************************************************************/


header_type ethernet_t {
    fields {
        dstAddr : 48;
        srcAddr : 48;
    }
}

header_type type_1_t {
    fields {
        type_1 : 16;      
    }    
}

header_type sr_hdr_t {
    fields {
        next_hdr : 1;
        rsvd : 7;
        sr_length : 8;
    }
}

header_type sr_t {   
    fields {
        sr_type : 1;    
        rsvd : 7;
        next_port : 8;
    }
}

header_type type_2_t {
    fields {
        type_2 : 16;       
    }    
}

header_type int_option_t {   
    fields {
        int_type : 1;    
        rsvd : 7;          
        int_num : 8; 
    }      
}

header_type inthdr_t {   
    fields {
        sw_id : 8;
        ingress_port : 8;
        egress_port : 8;
        rsvd0 : 8;
        ingress_global_timestamp : 48;
        egress_global_timestamp : 48;
        enq_timestamp : 32;
        rsvd1 : 8;
        enq_qdepth : 24;
        deq_timedelta : 32;
        rsvd2 : 8;
        deq_qdepth : 24;
    } 
}

header ethernet_t   ethernet;
header type_1_t     type_1;
header sr_hdr_t     sr_hdr;
header sr_t         sr[5];
header type_2_t     type_2;
header int_option_t int_option;
header inthdr_t     inthdr;

/********************************************************************************/
/********************************************************************************/

parser start {
    return parse_ethernet;
}

parser parse_ethernet {
    extract(ethernet);
    return parse_type_1;
}

parser parse_type_1 {
    extract(type_1);
    return parse_sr_hdr;
}

parser parse_sr_hdr {
    extract(sr_hdr);
    return parse_sr;
}

parser parse_sr {
    extract(sr[next]);
    return select(sr[last].sr_type) {
        0 : parse_sr;
        1 : parse_type_2;
    }
}

parser parse_type_2 {
    extract(type_2);
    return select(type_2.type_2) {
        0x701 : parse_int_option;
        0x800 : ingress;
    }
}

parser parse_int_option {
    extract(int_option);
    return select(int_option.rsvd) {
        0x11 : parse_inthdr;
        default : ingress;
    }
}

parser parse_inthdr {
    extract(inthdr);
    return ingress;
}

/********************************************************************************/
/********************************************************************************/

action _drop() {
    drop();
}

action _nop() {
}

action do_sr() {
    modify_field(standard_metadata.egress_spec, sr[0].next_port);
    pop(sr,1);
}

action do_remove() {
    remove_header(type_1);
    remove_header(sr_hdr);
}

action do_int() {
    add_header(inthdr);

    modify_field(inthdr.sw_id, 5);
    modify_field(inthdr.ingress_port, standard_metadata.ingress_port);
        

    add_to_field(int_option.int_num, 1);
}

table sr_table {
    actions {
        do_sr;
    }
    default_action: do_sr();
}

table remove_table {
    actions {
        do_remove;
    }
    default_action: do_remove();
}

table int_table {
    actions {
        do_int;
    }
    default_action: do_int();
}

/********************************************************************************/
/********************************************************************************/

control ingress {
    if(sr[0].sr_type==1) {
        apply(remove_table);
    }
    apply(sr_table);
}

control egress {
    if(valid(int_option)) {
        apply(int_table);
    }
}
