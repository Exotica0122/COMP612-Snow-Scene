# COMP612-Snow-Scene
A basic Snow Scene with C and OpenGL

First Assignment for COMP612 Computer Graphics at AUT.


https://user-images.githubusercontent.com/75343007/162339254-ac2533af-5b92-4447-b3c0-55ea5eae976f.mp4


This is a basic program made with procedural C and OpenGL with freeglut library.

OpenGL protoype functions used to separate the functionalities.


## Main


This is the entry point of the program used to define other prototypes


## Display


Used to process the displaying the scene. Only displaying functionality should be here.
Other processing like movement should go in the think function.


## Keypressed / Keyreleased


This functionaility is to control the keyboard input. Used to enable or disable features while in the program. Also for snowman control.


## Think


This is where all the heavy calculations happen to alter the values in the scene.
This calculates the movement for all particles and snowman so elements in the scene actually can move.


# Main Features:
- Scene display
- Drawing the Moon
- Drawing the Snowman
- Snoman movement
- Snow Particle System
- Star Particle System
- Shooting Star Particle System


