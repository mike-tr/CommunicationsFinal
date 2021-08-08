How to run tests:

cat node1.txt - | make run

to run node with settings from file node1.txt
then run:

cat node2.txt - | make run

Read the pdf file.
and yes this implementation can handle cycles.

we handle cycles by "marking" a node on "search"
if a node gets a discover message for node x, while also searching for x,
we imediatly send Nack message to the one asking the discover.
as we assume that they both trying to solver the same discover message.
clearly if the current node find the target node, the path that doesnt goes
via the asking node is shorter, hence we can simply "remove" the longer path from the start.