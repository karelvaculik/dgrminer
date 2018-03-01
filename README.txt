MANUAL FOR DGRMINER
===================


1) DESCRIPTION OF THE INPUT FORMAT:
-----------------------------------

================= Format of the input file for a single dynamic graph:
init
v ID_V LAB
u ID_E ID_F ID_T LAB
d ID_E ID_F ID_T LAB
changes
t # 1
an ID_V LAB
ae u ID_E ID_F ID_T LAB
ae d ID_E ID_F ID_T LAB
cn ID_V LAB
ce ID_E LAB
dn ID_V
de ID_E
t # 2
...
t # N
...
end
=================
where:
u = undirected
d = directed (the edge is oriented from ID_F to ID_T)
ID_V = vertex ID (integer)
ID_E = edge ID (integer)
ID_F = "from" vertex ID (integer)
ID_T = "to" vertex ID (integer)
LAB = label (string without space)

First line starts with "init"
After "init", there is a section describing the initial state of the graph.
This section can be empty, which means that the the graph is empty at the beginning.
By default, timestamp 0 will be appended to nodes and edges of this initial graph.

After initial section, there is another section, "changes".
Changes are grouped according to snapshots and each snapshot is stated by a line starting with "t".
First snaphot belongs to timestamp 1.
We allow following types of changes (at most one change can be applied to each element, i.e. node ID or edge ID, in a snapshot):

an ID_V LAB - add a node with id=ID_V and label LAB
ae u ID_E ID_F ID_T LAB - add an undirected edge with id=ID_E and label LAB between nodes with id=ID_F,ID_T
cn ID_V LAB - change label of node with id=ID_V to LAB
ce ID_E LAB - change label of edge with id=ID_E to LAB
dn ID_V - delete node with id=ID_V
de ID_E - delete edge with id=ID_E

All elements on each line must be separated by a space.

Multiedges are allowed, loops are not.

At the end, after all snapshots, there is a line with "end".

If analysing a set of dynamic graphs, use the same format as above for each graph and just concatenate all data together, e.g.:
================= CONTENT OF THE INPUT FILE:
init
...
end
init
...
end
...
...
...
init
...
end
=================

If there are at least 2 dynamic graphs in the file, DGRMiner will automatically recognize that a set of graphs is used.


2) PARAMETERS OF THE APPLICATION:
---------------------------------

-h, --help              Shows this help message
-i INPUTFILE            Specifies the input file
-o OUTPUTFILE           Specifies the common prefix for output files
-s K                    Specifies the minimum support; decimal value from [0.0, 1.0]
-c M                    Specifies the minimum confidence; decimal value from [0.0, 1.0]; if not specified, the confidence is not computed
-w N                    Specifies the size of window; 10 by default
-t {bin_nodes,bin_all}  Specifies the type of time abstraction
-a A                    Switches to anomaly detection; Only anomalies with outlierness >= A will be outputted; A is a decimal value from [0.0, 1.0]


3) DESCRIPTION OF THE OUTPUT FORMAT:
------------------------------------

Results with frequent patterns are written into 5 files with suffixes: _edges, _encoding, _measures, _nodes, _occurrences

_edges (list of all edges):
pattern = id of the pattern
from = id of "from-vertex"
to = id of "to-vertex"
label_int = integer id of label (use _encoding file for decoding the id)
direction = 0: undirected, 1: directed from "from-vertex" to "to-vertex", 2: directed from "to-vertex" to "from-vertex"
edgetime = relative timestamp

_encoding:
label = label string
label_int = label id
is_change = 1: label represents a change, 0: otherwise
label_int_prev = id of antecedent label (-1 if there is no antecedent)

_measures (one line for each pattern, corresponding to pattern id=1,2,3,...)
support_absolute = absolute support of the pattern
support_relative = relative support of the pattern
confidence = confidence of the pattern

_nodes (list of all nodes):
pattern = id of the pattern
id = vertex id
label_int = integer id of label (use _encoding file for decoding the id)
changetime = relative timestamp

_occurrences (one line for each pattern, corresponding to pattern_id=1,2,3,...):
graph_snapshots = list of numbers separated by commas; value k means that the rule represents a change from stapshot k-1 to k in the case of a single dynamic graph, or that the change occurred in the k-th dynamic graph in the case of a set of graphs

If anomalies are computed too, there are 5 extra files with suffixes: _anomalies_edges, _anomalies_explanation, _anomalies_nodes, _anomalies_occurrences, _anomalies_outlierness

_anomalies_edges, _anomalies_nodes and _anomalies_occurrences have same interpretation as in the case of frequent patterns.

_anomalies_explanation:
anomaly_pattern = id of the anomaly pattern
pattern = id of the explanation frequent pattern

_anomalies_outlierness (one line for each anomaly pattern, corresponding to anomaly pattern id=1,2,3,...):
outlierness = outlierness score of the pattern


4) EXAMPLES:
------------

Linux examples of running the program:
Frequent patterns:
./dgrminer -i intro_example -o intro_example_res -s 0.3 -c 0.0
./dgrminer -i intro_example -o intro_example_res -s 0.3 -c 0.0 -t bin_all

Anomalies:
./dgrminer -i intro_example -o intro_example_res -s 0.10 -c 0.00 -a 0.30
./dgrminer -i enron_multiedges_with_edge_ids -o enron_multiedges_with_edge_ids_10_60_80_res -s 0.10 -c 0.60 -a 0.80 -t bin_nodes

Windows examples:
DGRMiner.exe -i intro_example -o intro_example_res -s 0.3 -c 0.0
DGRMiner.exe -i intro_example -o intro_example_res -s 0.3 -c 0.0 -t bin_all


5) REFERENCES:
--------------

Vaculík, K.: A Versatile Algorithm for Predictive Graph Rule Mining. Proceedings ITAT 2015: Information Technologies - Applications and Theory. CEUR-WS.org, 2015.

Vaculík, K., Popelínský, L.: DGRMiner: Anomaly Detection and Explanation in Dynamic Graphs. Advances in Intelligent Data Analysis XV - 15th International Symposium, IDA 2016.


6) CHANGES SINCE ITAT'15 paper (changelog):
-------------------------------------------

v 1.1.0:
- deleted nodes and edges are not kept anymore, only as immediate change for rules (original approach is mentioned in section 2.1 of the paper)
- multiedges with the same labels are now allowed (edge IDs are used for "minimum code check" but not for pattern matching)
- if confidence is computed (and also antecedents), we cannot remove infrequent vertices and edges

v 1.1.1:
- resulting patterns are now written to the files immediately after they are found (previously, they all were written at the end of the computation)

v 2.0.0:
- added anomaly detection; see IDA'16 paper