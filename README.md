# INT-Probe: In-band Network-Wide Telemetry with Stationary Probe Generators and Collectors

Fine-grained, network-wide visibility is essential to reliably maintain and troubleshoot mega-scale modern data center networks to accommodate heterogeneous mission-critical applications. However, traditional management protocols, such as SNMP, fail to achieve the fine-grained monitoring due to the inefficient control path-driven polling mechanism, which cannot well adapt to the drastic traffic dyanmics in today's high-density data centers to promptly react to network issues. Some end host-driven approaches, such as Pingmesh, leverage servers to launch ping probes to provide the maximum latency measurement coverage. Although easy to deploy, they can hardly extract hop-by-hop latency or queue depth from network intermediate devices for deeper analysis, hindered by device's vertically-integrated black-box architecture. The protocol-independent forwarding architecture as well as P4 unleashes the power of data plane programmability and enables the network device to arbitrarily edit packet headers. The In-band Network Telemetry (INT) makes full use of this ability to allow probe packets to query device-internal states, such as queue depth and queuing latency, when they pass through the data plane pipeline. INT is in so great market demand that it has been embedded in vendors' latest merchant silicon. However, INT is essentially an underlying primitive for device-internal state exposure. For network-wide visibility, high-level orchestration on top of INT is further needed.

To achieve network-wide telemetry, one straightforward approach is to flood the INT probes into data center network's multi-rooted topology for entire network coverage. However, this leads to repetitive probing that one link will be monitored by many probes simultaneously with huge bandwidth waste. To overcome this limitation, we can rely on the controller for making centralized probing path coordination. INT-path proposed last year follows this design paradigm. Specifically, INT-path embeds source routing into INT probes to specify the route the probe packet takes through the network. Then, INT-path generates non-overlapped probing paths that cover the entire network with a minimum path number using an Euler trail-based path planning algorithm. INT-path is theoretically perfect but with severe deployment flaws. First, the probe device attachment points calculated by the algorithm are sensitive to the instant network topology. Any topology change will trigger the reattachment of probe devices, interrupting the 7*24 telemetry service. Second, according to the Euler trail-based algorithm, we should attach one probe device to each odd vertex in a network graph. However, in the worst case, all the vertices in a graph may be odd, which makes INT-path unaffordable. Third, INT-path treats all vertices equal, however, in the real world, not all the network intermediate devices are accessible/available to attach probe devices.

To tackle the above issues, we propose INT-probe, an In-band Network-Wide Telemetry architecture under the constraint of stationary probe devices. Unlike INT-path, the attachment points in INT-probe are confined to a predetermined subset of network devices (e.g., ToR switches) to avoid unstable reattachment during topology changes. Furthermore, we formulate the constrained path planning problem into an extended multi-depot k Chinese postman problem (MDCPP-set). The problem is sophisticated and we solve it by reducing it to the solvable minimum weight perfect matching. Actually, INT-path overemphasizes the 'non-overlapped' optimum, making path generation uncontrollable. If we could allow even a little path overlapping, the path generation process can be more controllable. INT-probe exactly follows such design philosophy, i.e., fix the path endpoints, find the least overlapped path planning!

The distinguishing features of INT-probe are listed as follows:

1. INT-probe is a theoretically optimal yet real-world deployable architecture for network-wide telemetry. By intentionally limiting the number and location of available attachment points, we can flexibly control the probe device CAPEX and the overall telemetry overhead.

2. The constrained path planning problem can be formulated into an extended multi-depot k Chinese postman problem (MDCPP-set), which is distinctive and has theoretical merit. We solve the problem by reducing it to a known problem of minimum weight perfect matching.

3. The extensive evaluation is done on random graphs, US backbone topology and two data center network topologies to show that INT-probe achieves network-wide, cost-effective, stable probing, ready for deployment. It reduces the path number by 81.70% by allowing only 1.22% increase of the total path length for a random graph.

# System

The system include three modules:p4app, packet and controller.

## p4app

Include p4 source code, implemented source-based INT functrion.

header.p4, parser.p4, app.p4: p4 source code

### header.p4

Including Headers and Metadatas

### parser.p4

Including parser, deparser and checksum calculator.

### app.p4

The main part of the p4 program. Including SR/INT/ARPProxy/UDP/ECMP tables.

### app.json

The json file that compiled from app.p4 by p4c compiler.

## packet:

Send & receive INT packet module which run in the hosts and the database config.

### int_data.sql

The SQL Table which is used to initalize the database.

### receiveint.c

Used to receive int packet, and parse them, then put them into database.

### sendint.py

Use the given traverse path generate the INT packet and encode SR info.

## controller

Generate Mininet network, send SR & INT command to hosts and get result from database.

### app.py

The main controller. Use topoGraph and hostList generate netwrok, then use path to traverse the netwrok and collect INT info.

### dbParser.py

The module which is used in app.py to get INT info from database.

### device.py

The module which is used in app.py to generate the virtual devices(Hosts/Switches).

### p4_mininet.py

The module which is used in mininiet to generate P4 devices.

### switchRuntime.py

The module which is used in app.py to down tables using thrift RPC.

### topoMaker.py

The module which is used in app.py to generate network topo.

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

# How to run

## Run system

If you installed the dependencies and configured the database(mysql,in`/system/packet/int_data.sql`) successfully, then you can run the system with commands below:

```
cd system/controller/
python app.py
```

You can change `graph`,`hostList` and `paths` in `app.py` to test your own network and you own INT path.

## Run Euler trail-based path planning algorithm

```
cd Euler_trail-based_path_planning_algorithm/algorithm/
python optimal_find_path_unbalance.py
```

## Run MDCPP_set algorithm

Firstly, you can generate the data files of different topologies using python2.7:

```
cd MDCPP_set/GraphGenerator/
```

Then, compile the MDCPP_set algorithm:

```
cd MDCPP_set/
make
```

Finally, run the MDCPP_set algorithm with the data files of topologies:

```
./testMDCPP topology.txt
```
