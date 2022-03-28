#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
/******************************************************************************
* Animation & Timing Setup
******************************************************************************/
// Target frame rate (number of Frames Per Second).
#define TARGET_FPS 60
// Ideal time each frame should be displayed for (in milliseconds).
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;
// Frame time in fractional seconds.
// Note: This is calculated to accurately reflect the truncated integer value of
// FRAME_TIME, which is used for timing, rather than the more accurate fractional
// value we'd get if we simply calculated "FRAME_TIME_SEC = 1.0f / TARGET_FPS".
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;
// Time we started preparing the current frame (in milliseconds since GLUT was 
// initialized).
unsigned int frameStartTime = 0;
/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/
 // Define all character keys used for input (add any new key definitions here).
 // Note: USE ONLY LOWERCASE CHARACTERS HERE. The keyboard handler provided 
 // converts all
 // characters typed by the user to lowercase, so the SHIFT key is ignored.
#define KEY_EXIT 27 // Escape key.
/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/
void displayText(char *text, float x, float y);
void displayAmountOfActiveParticles(void);

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void idle(void);
/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/
void main(int argc, char **argv);
void init(void);
void think(void);

//void spawnSnow(int position);
//void gravityEffect(Particle_t *snow);
//void windEffect(Particle_t *snow);
//void shakeEffect(Particle_t *snow);
//void decreaseTransparency(Particle_t *snow);
//void recycleSnow(Particle_t *snow);
/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/
#define PI 3.14159265
#define DEG_TO_RAD PI/180.0f

#define MAX_PARTICLES 1000

int frameCount = 0;

typedef struct {
	float x;
	float y;
} Vec2f;

typedef struct {
	float x;
	float y;
	float z;
} Vec3f;

typedef struct {
	Vec2f position;
	float velocity;
	int size;
	int isActive;
	Vec3f color;
	float transparency;
} Particle_t;

Particle_t snowSystem[MAX_PARTICLES];

Vec2f snowmanPosition = { 0.f, -0.6f };

int currentSnowPosition = 0;

int activeParticle = 0;

// flags
int isParticlesOn = 0;
int isWindOn = 0;
int isShakeOn = 0;

// This returns a random float between two values 
// Start and End are both inclusive
// [a, b]
float RandomFloat(float min, float max) {
	return ((max - min) * ((float)rand() / RAND_MAX)) + min;
}

//const float floorPositionX = RandomFloat(0.7f, 0.9f);
/******************************************************************************
 * Entry Point (don't put anything except the main function here)
 ******************************************************************************/
void main(int argc, char **argv)
{
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(300, 100);
	glutCreateWindow("Animation");
	// Set up the scene.
	init();
	// Disable key repeat (keyPressed or specialKeyPressed will only be called 
	// once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutIdleFunc(idle);
	// Record when we started rendering the very first frame (which should happen
	// after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);
	// Enter the main drawing loop (this will never return).

	glutMainLoop();
}
/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/
void displayBackground()
{
	glBegin(GL_POLYGON);
	{
		glColor4f(1.f, 1.f, 1.f, 0.8f);
		glVertex2f(-1, -1);
		glVertex2f(1, -1);
		glColor4f(0.2f, 0.6f, 1.f, 1.f);
		glVertex2f(1, 1);
		glVertex2f(-1, 1);
	}
	glEnd();
}

void displayFloor()
{
	glBegin(GL_POLYGON);
	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex2f(0.8f, -0.6f);
		glVertex2f(-0.8f, -0.6f);
		glColor3f(0.9f, 0.9f, 0.9f);
		glVertex2f(-1.5, -1.f);
		glVertex2f(1.5, -1.f);
	}
	glEnd();
}

void displaySnow(int position)
{
	for (unsigned int i = 0; i < MAX_PARTICLES; i++)
	{
		if (snowSystem[i].isActive == position)
		{
			glPointSize(snowSystem[i].size);
			glBegin(GL_POINTS);
			{
				glColor4f(snowSystem[i].color.x, snowSystem[i].color.y, snowSystem[i].color.z, snowSystem[i].transparency);
				glVertex2f(snowSystem[i].position.x, snowSystem[i].position.y);
			}
			glEnd();
		}
	}
}

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

void drawPentagon(float x, float y, float radius, Vec3f centerColor, Vec3f outerColor) {
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(centerColor.x, centerColor.y, centerColor.z);
	glVertex2f(x, y);
	glColor3f(outerColor.x, outerColor.y, outerColor.z);
	for (int theta = 0; theta <= 360; theta += 72)
	{
		glVertex2f((cos(theta * DEG_TO_RAD) * radius + x), (sin(theta * DEG_TO_RAD) * radius + y));
	}
	glEnd();
}

void drawSnowman(float x, float y)
{
	Vec3f white = { 1.f, 1.f, 1.f };
	Vec3f black = { 0.f, 0.f, 0.f };
	Vec3f lightBlue = { 0.68f, 0.85f, 0.9f };
	Vec3f orange = { 0.8f, 0.502f, 0.f };

	// body
	drawCircle(x, y - 0.225f, 0.15f, white, lightBlue);
	drawCircle(x, y, 0.125f, white, lightBlue);

	// left eye
	drawCircle(x - 0.04f, y + 0.05f, 0.015f, black, black);
	drawCircle(x - 0.04f, y + 0.05f, 0.005f, white, black);

	// right eye
	drawCircle(x + 0.04f, y + 0.05f, 0.015f, black, black);
	drawCircle(x + 0.04f, y + 0.05f, 0.005f, white, black);

	// nose
	drawPentagon(x, y, 0.0225f, white, orange);
}

void displayText(char *text, float x, float y)
{
	glColor3f(0.2f, 0.2f, 0.2f);
	glRasterPos2f(x, y);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, text);
}

void displayAmountOfActiveParticles()
{
	char particleString[40] = "particles: ";
	char activeParticleString[6];
	_itoa(activeParticle, activeParticleString, 10);
	strcat(particleString, activeParticleString);
	strcat(particleString, " of 1000");

	glColor3f(0.2f, 0.2f, 0.2f);
	glRasterPos2f(-0.98f, 0.9f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, particleString);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	frameCount++;

	// Scene display
	displayBackground();
	displayFloor();

	// Draw snows behind snowman
	displaySnow(1);

	// Snowman display
	drawSnowman(snowmanPosition.x, snowmanPosition.y);

	// Draw snows infront of snowman
	displaySnow(2);

	displayText("Diagnostics:", -1.f, 0.95f);

	displayAmountOfActiveParticles();

	displayText("Scene Controls:", -1.f, 0.85f);

	displayText("s: toggle snow", -0.98f, 0.8f);

	displayText("a: toggle shake", -0.98f, 0.75f);

	displayText("d: toggle wind", -0.98f, 0.7f);

	displayText("q: quit", -0.98f, 0.65f);

	glutSwapBuffers();
}


/*
Called when the OpenGL window has been resized.
*/
void reshape(int width, int h)
{
}


void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {
	case 's':
		if (isParticlesOn == 0) {
			currentSnowPosition = 0;
			isParticlesOn = 1;
		}
		else isParticlesOn = 0;
		break;

	case 'd':
		if (isWindOn == 0) isWindOn = 1;
		else if (isWindOn == 1) isWindOn = 2;
		else isWindOn = 0;
		break;

	case 'a':
		if (isShakeOn == 0) isShakeOn = 1;
		else isShakeOn = 0;
		break;

	case 'q':
		exit(0);
		break;

	case 'p':
		snowmanPosition.y += 0.01f;
		break;
	case ';':
		//do something here
		snowmanPosition.y -= 0.01f;
		break;
	case 'l':
		snowmanPosition.x -= 0.05f;
		break;
	case '\'':
		snowmanPosition.x += 0.05f;
		break;

	case KEY_EXIT:
		exit(0);
		break;
	}
}


void idle(void)
{
	// Wait until it's time to render the next frame.
	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) -
		frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		// This frame took less time to render than the ideal FRAME_TIME: we'll
		// suspend this thread for the remaining time
		// so we're not taking up the CPU until we need to render another 
		// frame.
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}
	// Begin processing the next frame.
	frameStartTime = glutGet(GLUT_ELAPSED_TIME); // Record when we started work on the new frame.
	think(); // Update our simulated world before the next call to display().
	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}
/******************************************************************************
 * Animation-Specific Functions (Add your own functions at the end of this section)
 ******************************************************************************/

void initialiseSnow(Particle_t *snow)
{
	snow->position.x = RandomFloat(-1.f, 1.f);
	snow->position.y = 1.f;

	snow->size = RandomFloat(3.f, 7.f);
	snow->velocity = snow->size / 1250.f;

	snow->isActive = 0;

	// r
	snow->color.x = 0.678f;
	// g
	snow->color.y = 0.847f;
	// b
	snow->color.z = 1.f;

	snow->transparency = RandomFloat(0.2f, 1.f);
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 1.0); //make the clear color black and opaque
	glColor3f(1.0, 1.0, 1.0); //set the drawing color to be white
	//set up our drawing area usinf default values
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

	// Initialize all particles before rendering the first frame
	for (unsigned int i = 0; i < MAX_PARTICLES; i++)
	{
		initialiseSnow(&snowSystem[i]);
	}
}
/******************************************************************************
* Animation functions for think function
 ******************************************************************************/

// postion=1 will spawn behind, position=2 will spawn in front of snowman
void spawnSnow(int position)
{
	// random snow amount between 0 and 8 will spawn
	int snowAmount = (int) RandomFloat(0, 8);

	int snowSpawnCounter = 0;

	// Activate 0 to 8 particles
	while (snowSpawnCounter < snowAmount && currentSnowPosition < MAX_PARTICLES) {
		if (snowSystem[currentSnowPosition].isActive == 0)
		{
			// Set it to be active behind or infront of snowman
			snowSystem[currentSnowPosition].isActive = position;

			// increment counters
			activeParticle++;
			snowSpawnCounter++;
		}
		currentSnowPosition++;
	}
}

void gravityEffect(Particle_t *snow)
{
	snow->position.y -= snow->velocity;
}

void windEffect(Particle_t *snow)
{
	float windVelocity = RandomFloat(3.f, 4.f);

	// Wind effect left
	if (isWindOn == 1)
	{
		snow->position.x += snow->velocity / (snow->position.y + windVelocity);
	}

	// Wind effect right
	if (isWindOn == 2)
	{
		snow->position.x -= snow->velocity / (snow->position.y + windVelocity);
	}
}

void shakeEffect(Particle_t *snow)
{
	snow->position.x += snow->velocity / RandomFloat(5.f, 8.f);
	snow->position.x -= snow->velocity / RandomFloat(5.f, 8.f);
}

void decreaseTransparency(Particle_t *snow)
{
	snow->transparency *= 0.95f;
}

void recycleSnow(Particle_t *snow)
{
	// reset positions and transparency
	snow->position.x = RandomFloat(-1.f, 1.f);
	snow->position.y = 1.f;
	snow->transparency = RandomFloat(0.2f, 1.f);

	// deactivate snow if turned off
	if (isParticlesOn == 0) {
		snow->isActive = 0;
		activeParticle--;
	}
}

void think(void)
{
	// Spawn particles every 10 frame
	if (frameCount % 10 == 0 && isParticlesOn == 1)
	{
		spawnSnow(1);
		spawnSnow(2);
	}

	for (unsigned int i = 0; i < MAX_PARTICLES; i++)
	{
		// if snow is activated
		if (snowSystem[i].isActive == 1 || snowSystem[i].isActive == 2)
		{
			gravityEffect(&snowSystem[i]);

			if (isWindOn != 0)
			{
				windEffect(&snowSystem[i]);
			}

			if (isShakeOn == 1)
			{
				shakeEffect(&snowSystem[i]);
			}

			// Gradually decrease transparency when below -0.75 y axis
			if (snowSystem[i].position.y < -0.75f)
			{
				decreaseTransparency(&snowSystem[i]);
			}

			// Reached maximum x or y level of particle destroy
			if (snowSystem[i].position.y < -0.97f || snowSystem[i].position.x == 1 || snowSystem[i].position.x == -1)
			{
				recycleSnow(&snowSystem[i]);
			}
		}
	}


}
/******************************************************************************/