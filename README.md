# DirectX11-Tank-Game
DirectX11 Tank Game

# The Aim of the Project
The enemy tank controlled by the computer finds and hits the tank used by the user in the shortest way.

By defining nodes to the coordinate points determined in the labyrinth, the neighborhood matrix is used to keep the distance values from the nodes neighbor to those nodes.

When calculating the Shortest Path, the Dijkstra algorithm found the shortest path values from the specified source node to other nodes. The shortest path was found by making a Breadth Search to the target node determined as a result of the found values.


![labirent](https://raw.githubusercontent.com/servetsatiroglu/DirectX11-Tank-Game/master/Labirent.png)
