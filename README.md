# Snow Scene
A basic Snow Scene with C and OpenGL

First Assignment for COMP612 Computer Graphics at AUT.


https://user-images.githubusercontent.com/75343007/162339254-ac2533af-5b92-4447-b3c0-55ea5eae976f.mp4


This is a basic program made with procedural C and OpenGL with freeglut library.

OpenGL protoype functions used to separate the functionalities.

**The coordinates for the window is:**
Bottom left: (-1, -1)
Top right: (1, 1)
Middle: (0, 0)


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

---

# Main Features:
- Scene display
- Drawing the Snowman
- Snowman movement
- Snow Particle System
- Star Particle System
- Shooting Star Particle System


**Scene display**

This is where the sunset color, ground, mountains and trees are displayed. sunset color is made with GL_POLYGON.

`
void displaySunset()
{
	glBegin(GL_POLYGON);
	{
		glColor4f(0.96f, 0.62f, 0.76f, 1.f);
		glVertex2f(-1, -0.6f);
		glVertex2f(1, -0.6f);
		glColor4f(0.f, 0.f, 0.f, 0.5f);
		glVertex2f(1, -0.1f);
		glVertex2f(-1, 0.2f);
	}
	glEnd();
}
`
  
Using by-vertex, The colors will interpolate to make a sunset look.
  
Ground is spawned randomly everytime when reloading the scene. Uses GL_POLYGON too to make a trapezoid shape. Same logic as above.
  
The moon is made using GL_LINE_STRIP. Two different angles are used to give a cresent moon look.
  
Used a reference from stackoverflow. Altered the code so it can change scale and be moved around.
  
https://stackoverflow.com/questions/7260963/drawing-a-crescent-shape-in-opengl.


**Drawing the Snowman**

Most of the logic for drawing the snowman required to draw a circle. Here is the functionality for drawing a circle.

`
void drawCircle(float x, float y, float radius, Vec3f centerColor, Vec3f outerColor) {
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(centerColor.x, centerColor.y, centerColor.z);
	glVertex2f(x, y);
	glColor3f(outerColor.x, outerColor.y, outerColor.z);
	for (int theta = 0; theta <= 360; theta += 10)
	{
		glVertex2f((cos(theta * DEG_TO_RAD) * radius + x), (sin(theta * DEG_TO_RAD) * radius + y));
	}
	glEnd();
}
`

This allows to be moved around anywhere and the scale (radius) to be changed.

Once the circle can be drawn and moved, making the snowman is simple as move the x,y coordinates and the scale.


**Snowman movement**

This snowman is moved with the arrow keys. This meant I had to enable the special key functions for OpenGL

`
void keyPressed(unsigned char key, int x, int y);
void keyReleased(unsigned char key, int x, int y);
void specialKeyPressed(unsigned char key, int x, int y);
`

Once the prototypes and functions are created a flag for each arrow key was made to detect if the user is pressed or released the key.

Moving the snowman meant the x,y coordinates needs to change. When moving backwards the scale of the snowman changes so it gives a fake depth feel to the scene.

**Snow Particle System**

This was the main part of the assignment. The snow particles will have random x position but will alway spawn at the top of the window if activated.
Size and transparency is also randomized.
A shake effect and wind effect can be added with the keyboard buttons.

Once the snow is actived by pressing 's' it will start to display in the scene, spawn at the top and fall down slowly.

Once reaching below -0.75 in the y, the snow will slowly decrease transparency. When reaching below 0.97 or outside of the window in the left or right direction, the snow will reset back up (y=1) and the x position will randomize. All other variables will remain the same.

If the user deactivates the snow system then the snow particle will stop spawning. Only when the snow hits below 0.97 is when the snow will deactivate in the scene.
