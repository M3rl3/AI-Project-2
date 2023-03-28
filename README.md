# Artificial Intelligence Project-2

Project 2 for GDP-6017 (Artificial Intelligence)

Build Instructions:
- Built using Visual Studio 17 (2022) - Retarget solution if necessary.
- All build requirements and other files are included within the project and solution directories.

General Information:
- Following project implements the A* search algorithm to traverse throught a landscape.
- The map is generated from a BMP image file of size 64x64.
- The algorithm places walls based on the color value of the pixels.
- So a black pixel would warrant a wall.
- White pixel is just free space.
- Red and Green pixels are the Goal and Start nodes respectively. There can only be exactly one goal and one start node in an image.
- The agent can go East, West, North, South, NE, NW, SE, SW. So long as there is nothing obstructing its path.
- However, the agent cannot go directly diagonal (NE, NW, SE, SW), if there is an obstruction on any immediate side.
- The algorithm will find a path through any and all BMP files as long as they have a valid (non-blocked) route and are within above mentioned specifications.

Controls:
- The camera will always be pointed at the agent that is traversing the landscape.
- Pressing F1 will enable controlling the camera with the mouse and moving it with the regular directional keys (W,A,S,D).
- Pressing and holding Left Alt will briefly display the cursor (minimize/maximize the window).