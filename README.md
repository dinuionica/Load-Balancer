# Load-Balancer
Based on data structures, a load-balancer has been implemented.
This is a mechanism frequently used in distributed systems and has the advantage 
of meeting minimal disruption constraint, ie minimizing the number of transfers 
required when a server is down or one is turned on. Specifically, when a server
is shut down, only the objects on it need to be redistributed to nearby servers.
Similarly, when a new server is added, it will only retrieve objects from a l
imited number of neighboring servers.
