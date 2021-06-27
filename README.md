# Load-Balancer
Based on data structures, a load-balancer has been implemented.
This is a mechanism frequently used in distributed systems and has the advantage 
of meeting minimal disruption constraint, ie minimizing the number of transfers 
required when a server is down or one is turned on. Specifically, when a server
is shut down, only the objects on it need to be redistributed to nearby servers.
Similarly, when a new server is added, it will only retrieve objects from a l
imited number of neighboring servers.

For download this project:
```
https://github.com/dinuionica/Load-Balancer.git
```
For test you can use make command using linux :<br />
```
make 
```
Implementation:

In solving the topic I chose to use a circular list
double chained for load_balancer implementation,
because accessing the neighbors of a node can be done in a
efficiently. In representation, each node of the hash ring
contains in addition to the two pointers next and prev, a pointer to
a server structure that contains a hashtable, an id, and a hash
of a replica. For the server side I used a hashtable
which contains an array of simply chained lists.


# Server:

For this part I used the skeleton and the implemented functions
within the lab of simply chained and hashtable lists.

Functions:
init_server_memory-> Within this function I created and I
initialized the server, respectively its hashtable

serve_store-> In this function I called the function which
add a new key-value pair to the server hashtable

server_remove-> Within this function I called the function
to delete a value from the desired server hashtable

server_retrive-> Within this function I extracted an object
based on a key transmitted as a parameter in the server hashtable

free_server_memory-> In this function I released first
the memory of a hastable, then I freed the memory allocated to the server
and I redirected the pointer to NULL


# Load-Balancer:

For this part I used the skeleton and the implemented functions
in the double-chain circular list laboratory.

Functions:
init_load_balancer-> Within this function we have allocated load
the balancer, I created and initialized the hash_ring in the form
of double-stranded circular list.

load_store-> In this function I went through the list, and
if I found a valid server I add an object with the key and the value
transmitted as a parameter in the desired server hashtable

load_retrive-> Similar to the function above I went through the list,
and if I found a valid server I extracted the value of the object
with the desired key in the hashtable

load_add_server-> In this function I created and initialized
a server, and based on three different replicas, using hashing
I added it three times in the hash_ring.

load_remove_server-> In this function I called three
or the function that deletes a replica of a server from the hash_ring

free_load_balancer-> In this function I went through the list
and I deleted the replicas, then called the list-specific function
which frees up the memory of each node. Finally I released load
the balancer and I redirected the pointer to NULL

add_server-> In this function I checked if the list
it is empty and if so I added a replica to a server
at the top of the list. Otherwise, I went through the list and counted on
how many positions the server hash is smaller than whose
server we want to add it. Then I added the node to the list
(dll_add_nth_node returns a pointer to the position I have
added one to not scroll through the list multiple times),
and then I searched and distributed objects from neighboring servers
to the newly added server.

remove_server-> Similar to the function above, I went through the list
on determining the position from which a server should be deleted. Then
I extracted the desired node from the hash_ring and redistributed the objects
to the neighboring servers. Finally I deleted the hastable
server only once (last replica) and I removed the replica
within the hash ring using the dll_remove_nth_node function and
initially determined position.

I used the macro for memory allocation errors
DIE taken from the header utils.h
