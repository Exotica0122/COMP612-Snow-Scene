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

Particle_t particleSystem[1000];
int currentSnowPosition = 0;
int activeParticle = 0;
int isParticlesOn = 0;
int isWindOn = 0;
int isShakeOn = 0;

// This returns a random float between two values 
// Start and End are both inclusive
// [a, b]
float RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

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
void display(void)
{
	frameCount++;

	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw snows behind snowman
	for (unsigned int i = 0; i < sizeof(particleSystem) / sizeof(particleSystem[0]); i++)
	{
		Particle_t currentSnow = particleSystem[i];
		if (currentSnow.isActive == 1)
		{
			glPointSize(currentSnow.size);
			glBegin(GL_POINTS);
			{
				glColor4f(currentSnow.color.x, currentSnow.color.y, currentSnow.color.z, currentSnow.transparency);
				glVertex2f(currentSnow.position.x, currentSnow.position.y);
			}
			glEnd();
		}
	}

	// Draw snows infront of snowman
	for (unsigned int i = 0; i < sizeof(particleSystem) / sizeof(particleSystem[0]); i++)
	{
		Particle_t currentSnow = particleSystem[i];
		if (currentSnow.isActive == 2)
		{
			glPointSize(currentSnow.size);
			glBegin(GL_POINTS);
			{
				glColor4f(currentSnow.color.x, currentSnow.color.y, currentSnow.color.z, currentSnow.transparency);
				glVertex2f(currentSnow.position.x, currentSnow.position.y);
			}
			glEnd();
		}
	}

	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(-1.f, 0.95f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, "Diagnostics:");

	char particleString[40] = "particles: ";
	char activeParticleString[4];
	_itoa(activeParticle, activeParticleString, 10);
	strcat(particleString, activeParticleString);


	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(-1.f, 0.90f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, particleString);

	glutSwapBuffers();
	/*
	TEMPLATE: REPLACE THIS COMMENT WITH YOUR DRAWING CODE
	Separate reusable pieces of drawing code into functions, which you can
	add
	to the "Animation-Specific Functions" section below.
	Remember to add prototypes for any new functions to the "Animation-
	Specific
	Function Prototypes" section near the top of this template.
	*/
}
/*
Called when the OpenGL window has been resized.
*/
void reshape(int width, int h)
{
}
/*
Called each time a character key (e.g. a letter, number, or symbol) is
pressed.
*/
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {
		/*
		TEMPLATE: Add any new character key controls here.
		Rather than using literals (e.g. "d" for diagnostics), create a
		new KEY_
		definition in the "Keyboard Input Handling Setup" section of this
		file.
		*/
	case 's':
		if (isParticlesOn == 0) isParticlesOn = 1;
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
/*
Called by GLUT when it's not rendering a frame.
Note: We use this to handle animation and timing. You shouldn't need to
modify
this callback at all. Instead, place your animation logic (e.g. moving or
rotating
things) within the think() method provided with this template.
*/
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
 /*
 Initialise OpenGL and set up our scene before we begin the render loop.
 */

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 1.0); //make the clear color black and opaque
	glColor3f(1.0, 1.0, 1.0); //set the drawing color to be white
	//set up our drawing area usinf default values
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

	// Initialize all particles before rendering the first frame
	for (unsigned int i = 0; i < sizeof(particleSystem) / sizeof(particleSystem[0]); i++)
	{
		// Position
		Vec2f startPosition;
		startPosition.x = RandomFloat(-1.f, 1.f);
		startPosition.y = 1.f;

		// Color
		Vec3f snowColor;
		// Red
		snowColor.x = 0.678f;
		// Green
		snowColor.y = 0.847f;
		// Blue
		snowColor.z = 1.f;

		// Size (Affects Gravity)
		int size = RandomFloat(3.f, 10.f);

		// Initialize rest of the values
		particleSystem[i].position = startPosition;
		particleSystem[i].velocity = size / 2000.f;
		particleSystem[i].size = size;
		particleSystem[i].isActive = 0;
		particleSystem[i].color = snowColor;
		particleSystem[i].transparency = RandomFloat(0.2f, 1.f);
	}
}
/*
Advance our animation by FRAME_TIME milliseconds.
Note: Our template's GLUT idle() callback calls this once before each new
frame is drawn, EXCEPT the very first frame drawn after our application
starts. Any setup required before the first frame is drawn should be placed
in init().
*/


void think(void)
{
	// Spawn 10 particles every 10 frame
	if (frameCount % 10 == 0 && isParticlesOn == 1)
	{
		if (particleSystem[currentSnowPosition].isActive == 0)
		{
			// Activate 5 particles for behind snowman
			for (unsigned int i = 0; i < 8; i++) {
				// Set it to be active 
				particleSystem[currentSnowPosition].isActive = 1;

				// increment counters
				activeParticle++;
				currentSnowPosition++;

				// Back to 0 when reached last element of array
				if (currentSnowPosition >= sizeof(particleSystem) / sizeof(particleSystem[0])) currentSnowPosition = 0;
			}

			// Activate 5 particles for infront snowman
			for (unsigned int i = 0; i < 8; i++) {
				// Set it to be active front of snowman
				particleSystem[currentSnowPosition].isActive = 2;

				// increment counters
				activeParticle++;
				currentSnowPosition++;

				// Back to 0 when reached last element of array
				if (currentSnowPosition >= sizeof(particleSystem) / sizeof(particleSystem[0])) currentSnowPosition = 0;
			}
		}
	}

	for (unsigned int i = 0; i < sizeof(particleSystem) / sizeof(particleSystem[0]); i++)
	{
		// Velocity of the snow particle
		if (particleSystem[i].isActive == 1 || particleSystem[i].isActive == 2)
		{
			particleSystem[i].position.y -= particleSystem[i].velocity;

			// Wind effect left
			if (isWindOn == 1) 
			{
				particleSystem[i].position.x += particleSystem[i].velocity / RandomFloat(10.f, 30.f);
			}

			// Wind effect right
			if (isWindOn == 2)
			{
				particleSystem[i].position.x -= particleSystem[i].velocity / RandomFloat(10.f, 30.f);
			}

			// Shake effect
			if (isShakeOn == 1)
			{
				particleSystem[i].position.x += particleSystem[i].velocity / RandomFloat(2.f, 5.f);
				particleSystem[i].position.x -= particleSystem[i].velocity / RandomFloat(2.f, 5.f);
			}

			// Gradually decrease transparency when below -0.75 y axis
			if (particleSystem[i].position.y < -0.75f)
			{
				particleSystem[i].transparency *= 0.95f;
			}

			// Reached maximum x or y level of particle destroy
			if (particleSystem[i].position.y < -0.97f || particleSystem[i].position.x == 1)
			{
				particleSystem[i].isActive = 0;
				activeParticle--;

				Vec2f newStartPosition;
				newStartPosition.x = RandomFloat(-1.f, 1.f);
				newStartPosition.y = 1.f;

				particleSystem[i].position = newStartPosition;
				particleSystem[i].transparency = RandomFloat(0.2f, 1.f);
			}
		}
	}
}
/******************************************************************************/