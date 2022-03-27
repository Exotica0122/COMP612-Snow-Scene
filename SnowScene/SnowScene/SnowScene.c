/******************************************************************************
 *
 * Animation v1.0 (23/02/2021)
 *
 * This template provides a basic FPS-limited render loop for an animated scene.
 *
 ******************************************************************************/
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
/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/
#define MAX_PARTICLES = 1000

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

typedef struct {
	Particle_t particle;
	int length;
	int start;
	int end;
};

Particle_t snowSystem[1000];

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
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = max - min;
	float r = random * diff;
	return min + r;
}

// display text variable

/******************************************************************************
 * Entry Point (don't put anything except the main function here)
 ******************************************************************************/
void main(int argc, char **argv)
{
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1000, 800);
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
 /*
 Called when GLUT wants us to (re)draw the current animation frame.
 Note: This function must not do anything to update the state of our
simulated
 world. Animation (moving or rotating things, responding to keyboard input,
 etc.) should only be performed within the think() function provided below.
 */

void displaySnow(int position)
{
	for (unsigned int i = 0; i < sizeof(snowSystem) / sizeof(snowSystem[0]); i++)
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

void displayText(char *text, float x, float y)
{
	glColor3f(1.0, 1.0, 1.0);
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

	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(-0.98f, 0.9f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, particleString);
}

void display(void)
{
	frameCount++;

	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw snows behind snowman
	displaySnow(1);

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

	int size = RandomFloat(3.f, 7.f);
	snow->size = size;
	snow->velocity = size / 1250.f;

	snow->isActive = 0;

	snow->color.x = 0.678f;
	snow->color.y = 0.847f;
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
	for (unsigned int i = 0; i < sizeof(snowSystem) / sizeof(snowSystem[0]); i++)
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
	int snowAmount = (int) RandomFloat(0, 8);

	int snowSpawnCounter = 0;

	// Activate 0 to 8 particles
	while (snowSpawnCounter < snowAmount && currentSnowPosition < sizeof(snowSystem) / sizeof(snowSystem[0])) {
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
	int windVelocity = RandomFloat(5.f, 20.f);

	// Wind effect left
	if (isWindOn == 1)
	{
		snow->position.x += snow->velocity / windVelocity ;
	}

	// Wind effect right
	if (isWindOn == 2)
	{
		snow->position.x -= snow->velocity / windVelocity;
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

	for (unsigned int i = 0; i < sizeof(snowSystem) / sizeof(snowSystem[0]); i++)
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