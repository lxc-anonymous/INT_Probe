# INT-Probe: In-band Network-Wide Telemetry with Stationary Probe Generators and Collectors

Fine-grained, network-wide visibility is essential to reliably maintain and troubleshoot mega-scale modern data center networks to accommodate heterogeneous mission-critical applications. However, traditional management protocols, such as SNMP, fail to achieve the fine-grained monitoring due to the inefficient control path-driven polling mechanism, which cannot well adapt to the drastic traffic dyanmics in today's high-density data centers to promptly react to network issues. Some end host-driven approaches, such as Pingmesh, leverage servers to launch ping probes to provide the maximum latency measurement coverage. Although easy to deploy, they can hardly extract hop-by-hop latency or queue depth from network intermediate devices for deeper analysis, hindered by device's vertically-integrated black-box architecture. The protocol-independent forwarding architecture as well as P4 unleashes the power of data plane programmability and enables the network device to arbitrarily edit packet headers. The In-band Network Telemetry (INT) makes full use of this ability to allow probe packets to query device-internal states, such as queue depth and queuing latency, when they pass through the data plane pipeline. INT is in so great market demand that it has been embedded in vendors' latest merchant silicon. However, INT is essentially an underlying primitive for device-internal state exposure. For network-wide visibility, high-level orchestration on top of INT is further needed.

To achieve network-wide telemetry, one straightforward approach is to flood the INT probes into data center network's multi-rooted topology for entire network coverage. However, this leads to repetitive probing that one link will be monitored by many probes simultaneously with huge bandwidth waste. To overcome this limitation, we can rely on the controller for making centralized probing path coordination. INT-path proposed last year follows this design paradigm. Specifically, INT-path embeds source routing into INT probes to specify the route the probe packet takes through the network. Then, INT-path generates non-overlapped probing paths that cover the entire network with a minimum path number using an Euler trail-based path planning algorithm. INT-path is theoretically perfect but with severe deployment flaws. First, the probe device attachment points calculated by the algorithm are sensitive to the instant network topology. Any topology change will trigger the reattachment of probe devices, interrupting the 7*24 telemetry service. Second, according to the Euler trail-based algorithm, we should attach one probe device to each odd vertex in a network graph. However, in the worst case, all the vertices in a graph may be odd, which makes INT-path unaffordable. Third, INT-path treats all vertices equal, however, in the real world, not all the network intermediate devices are accessible/available to attach probe devices.

To tackle the above issues, we propose INT-probe, an In-band Network-Wide Telemetry architecture under the constraint of stationary probe devices. Unlike INT-path, the attachment points in INT-probe are confined to a predetermined subset of network devices (e.g., ToR switches) to avoid unstable reattachment during topology changes. Furthermore, we formulate the constrained path planning problem into an extended multi-depot k Chinese postman problem (MDCPP-set). The problem is sophisticated and we solve it by reducing it to the solvable minimum weight perfect matching. Actually, INT-path overemphasizes the 'non-overlapped' optimum, making path generation uncontrollable. If we could allow even a little path overlapping, the path generation process can be more controllable. INT-probe exactly follows such design philosophy, i.e., fix the path endpoints, find the least overlapped path planning!

The distinguishing features of INT-probe are listed as follows:

1. INT-probe is a theoretically optimal yet real-world deployable architecture for network-wide telemetry. By intentionally limiting the number and location of available attachment points, we can flexibly control the probe device CAPEX and the overall telemetry overhead.

2. The constrained path planning problem can be formulated into an extended multi-depot k Chinese postman problem (MDCPP-set), which is distinctive and has theoretical merit. We solve the problem by reducing it to a known problem of minimum weight perfect matching.

3. The extensive evaluation is done on random graphs, US backbone topology and two data center network topologies to show that INT-probe achieves network-wide, cost-effective, stable probing, ready for deployment. It reduces the path number by 81.70% by allowing only 1.22% increase of the total path length for a random graph.

# System
The system includes six modules: `bmv2_model`, `controller`, `flow_table`, `p4_source_code`, `packet`, `topology`.

****

## bmv2_model
The bmv2 target used in the network. 
### simple_switch
Simple_switch is one of the P4 software switch targets.

****

## p4_source_code
The P4 file which defines the packet processing logic of the switches.
### my_int.py
Line 9-51: the header definition.
Line 57-99: the parse for each type of packets.
Line 113-221: the match-action field. The switch will forward the data packets according to its SR field and add the INT header into the INT probes.
### my_int.json
The output of compiling `my_int.p4`.
### Others
Not used.

****

## topology
Create the virtual network.
### clos.py
Create the clos architecture network with customied scale.
### p4_mininet.py
The reference for adding P4 switches into the network.

****

## flow_table
Include the flow tables, flow table generator.
### ./flow_table
The flow tables.
### ./flow_table/flow_table_gen.py
Generate the flow tables for customized clos architecture topology. And the output files will be in `flow_table`.
### ./flow_table/command.sh
Dump the flow tables into the P4 switches.
### ./flow_table/simple_switch_CLI
The control plane of simple switches.

****

## packet
Include the packet sending and receiving scripts.
### ./send
The packet sending scripts.
### ./send/send_int_probe.py
Send int probes.
### ./send/send_udp.py
Send data packets.
### ./receive
The packet receiving and processing scripts.
### ./receive/parse.py
Parse the packets.
### ./receive/receive.py
Receive all packets and use `parse.py` to parse the packets. And store the INT information into the database.
### ./receive/processor.py
Not used.

****

## controller
The source code of the controller.
### controller.py
The controller will subscribe the Redis database and catches all the expire events. And use these information for failure localization.

***

# Requisite third parties
P4 development enviornment: behavioral_model, p4c  
Database: Redis. And modify the configuration of redis to enable the unix socket and sub/pub function.


# Euler trail-based path planning algorithm

## algorithm

### optimal_find_path_balance.py

Heuristic algorithm for balance task.

### optimal_find_path_unbalance.py

Heuristic algorithm for unbalance task.

## figure_generation

### random graph generator new.py

Heuristic algorithm for generating graph.

### randomTopo.py

Heuristic algorithm for generating graph.

### specialTopo.py

Heuristic algorithm for generating special graph, such as FatTree and SpineLeaf topo.

# MDCPP_set algorithm

## algorithm

### MDCPPWithSet.h / MDCPPWithSet.cpp

Define MDCPPWithSet class.

### testMDCPP.cpp

Main function to test MDCPPWithSet algorithm.

### MWPM

The project of Blossom V, which is to solve the minimum weight perfect matching problem.

## figure_generation

### GraphGenerator

This folder includes some solutions to generate different topologies and depots. The python files prefixed with SNDlibGraphGen* can generate US backbone network topology. The files prefixed with randomGraphGen* generate graphs of different sizes randomly. fatTreeGen.py and spineLeafGen.py are both classic data center network topologies.
