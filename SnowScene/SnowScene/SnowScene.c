#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
/******************************************************************************
* Animation & Timing Setup
******************************************************************************/
#define TARGET_FPS 60
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;
unsigned int frameStartTime = 0;
/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/
#define KEY_EXIT 27 // Escape key.

// Special movement keys
#define KEY_ARROW_UP 101
#define KEY_ARROW_DOWN 103
#define KEY_ARROW_RIGHT 102
#define KEY_ARROW_LEFT 100
/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/
void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void keyReleased(unsigned char key, int x, int y);
void specialKeyPressed(unsigned char key, int x, int y);
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

// Circle helpers
#define PI 3.14159265
#define DEG_TO_RAD PI/180.0f

// Particle system length
#define MAX_SNOW_PARTICLES 1000
#define MAX_STAR_PARTICLES 50

// Frame counter
int frameCount = 0;

// Structs for systems
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
	Vec2f position;
	GLfloat theta;
	float velocity;
	float transparency;
	float initialScale;
	float scale;
	int lineSize;
	int scaleDirection;
	int transparencyDirection;
	int isActive;
} Particle_S;

typedef struct {
	Vec2f lightPosition;
	Vec2f darkPosition;
	float lineWidth;
	float velocity;
	int isActive;
} Particle_SS;


typedef struct {
	Vec2f position;
	float scale;
} Snowman;


// Snow system
Particle_t snowSystem[MAX_SNOW_PARTICLES];
int currentSnowPosition = 0;
int activeParticle = 0;

// Star system
Particle_S starSystem[MAX_STAR_PARTICLES];
int currentStarPosition = 0;

// Shooting Stars
Particle_SS shootingStar;

// Position values
Snowman snowman;
Vec2f groundPosition; // = { 0.8f, -0.6f };

// Flags
int isParticlesOn = 0;
int isWindOn = 0;
int isShakeOn = 0;
int isStarOn = 0;

// Keyboard flags
int arrowUpOn = 0;
int arrowDownOn = 0;
int arrowLeftOn = 0;
int arrowRightOn = 0;

// This returns a random float between two values 
// Start and End are both inclusive
// [a, b]
float RandomFloat(float min, float max) {
	return ((max - min) * ((float)rand() / RAND_MAX)) + min;
}

/******************************************************************************
 * Entry Point (don't put anything except the main function here)
 ******************************************************************************/
void main(int argc, char **argv)
{
	// Random number seeding
	srand(time(0));

	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(300, 100);
	glutCreateWindow("Animation");
	// Set up the scene.
	init();
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutSpecialUpFunc(keyReleased);
	glutSpecialFunc(specialKeyPressed);
	glutIdleFunc(idle);
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);

	glutMainLoop();
}
/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/
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

void displayTree(float x, float y, float scale)
{
	glBegin(GL_TRIANGLES);
	{
		glVertex2f(x - 0.03 * scale, y);
		glVertex2f(x, y + 0.14f * scale);
		glVertex2f(x + 0.03 * scale, y);
	}
	glEnd();
}

void displayBackground()
{
	glBegin(GL_TRIANGLES);
	{
		glColor3f(0.09f, 0.05f, 0.24f);
		glVertex2f(-1.5f, -1.f);
		glVertex2f(-0.3f, -0.2f);
		glVertex2f(0.6f, -0.6f);

		glColor3f(0.09f, 0.05f, 0.29f);
		glVertex2f(1.5f, -1.f);
		glVertex2f(0.2f, -0.2f);
		glVertex2f(-0.8f, -0.6f);

		glColor3f(0.07f, 0.04f, 0.26f);
		glVertex2f(-1.5f, -0.7f);
		glVertex2f(-0.8f, -0.25f);
		glVertex2f(0.5f, -1.f);

		glVertex2f(1.5f, -1.f);
		glVertex2f(0.8f, -0.35f);
		glVertex2f(0.2f, -0.8f);
	}
	glEnd();
		
	displayTree(0.9f, -0.47f, 0.7f);
	displayTree(0.55f, -0.55f, 0.6f);
	displayTree(0.45f, -0.62f, 0.5f);

	glColor3f(0.02f, 0.f, 0.19f);
	// closest mountain
	glBegin(GL_TRIANGLES);
	{
		glVertex2f(-1.5f, -1.f);
		glVertex2f(-0.4f, -0.25f);
		glVertex2f(1.f, -1.f);
	}
	glEnd();
		
	// closest trees
	displayTree(-0.85f, -0.6f, 1.f);
	displayTree(-0.78f, -0.55f, 0.8f);
	displayTree(-0.55f, -0.38f, 0.6f);
	displayTree(-0.2f, -0.38f, 0.8f);
	displayTree(0.f, -0.5f, 0.7f);
	displayTree(0.2f, -0.6f, 1.f);
	
}


void displayFloor()
{
	glBegin(GL_POLYGON);
	{
		glColor3f(0.f, 0.f, 0.f);
		glVertex2f(groundPosition.x, groundPosition.y);
		glVertex2f(-groundPosition.x, groundPosition.y);
		glColor3f(0.09f, 0.05f, 0.24f);
		glVertex2f(-1.5, -1.f);
		glVertex2f(1.5, -1.f);
	}
	glEnd();
}

void displaySnow(int position)
{
	for (unsigned int i = 0; i < MAX_SNOW_PARTICLES; i++)
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

void drawStarLines(float x, float y, GLfloat theta, float scale, int angle, float lineSize, float transparency)
{
	glLineWidth(lineSize);
	glBegin(GL_LINES);
	{
		glColor4f(1, 1, 1, 0);
		glVertex2f((float)cos(DEG_TO_RAD * (theta + angle)) * scale + x,
			(float)sin(DEG_TO_RAD * (theta + angle)) * scale + y);
		glColor4f(1, 1, 1, transparency);
		glVertex2f(x, y);
	}
	glEnd();
}

void displayStar()
{
	for (unsigned int i = 0; i < MAX_STAR_PARTICLES; i++) 
	{
		for (int y=0; y < 360; y+=45) {
			if(starSystem[i].isActive == 1)
			drawStarLines(starSystem[i].position.x, starSystem[i].position.y, starSystem[i].theta, 
				starSystem[i].scale, y, starSystem[i].lineSize, starSystem[i].transparency);
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

// Draws snowman at the bottom of its sprite
// x,y will be the bottom edge of snowmans body
void drawSnowman(float x, float y)
{
	Vec3f white = { 1.f, 1.f, 1.f };
	Vec3f black = { 0.f, 0.f, 0.f };
	Vec3f lightBlue = { 0.68f, 0.85f, 0.9f };
	Vec3f orange = { 0.8f, 0.502f, 0.f };

	// body
	drawCircle(x, (y + 0.15f) * snowman.scale, snowman.scale * 0.15f, white, lightBlue);
	drawCircle(x, (y + 0.35f) * snowman.scale, snowman.scale * 0.125f, white, lightBlue);

	// left eye
	drawCircle(x - 0.04f * snowman.scale, (y + 0.39f) * snowman.scale, snowman.scale * 0.015f, black, black);
	drawCircle(x - 0.04f * snowman.scale, (y + 0.39f) * snowman.scale, snowman.scale * 0.005f, white, black);

	// right eye
	drawCircle(x + 0.04f * snowman.scale, (y + 0.39f) * snowman.scale, snowman.scale * 0.015f, black, black);
	drawCircle(x + 0.04f * snowman.scale, (y + 0.39f) * snowman.scale, snowman.scale * 0.005f, white, black);

	// nose
	drawPentagon(x, (y + 0.35f) * snowman.scale, snowman.scale * 0.0225f, white, orange);
}

void displayText(char *text, float x, float y)
{
	glColor3f(1.f, 1.f, 1.f);
	glRasterPos2f(x, y);
	glutBitmapString(GLUT_BITMAP_8_BY_13, text);
}

void displayAmountOfActiveParticles()
{
	char particleString[40] = "particles: ";
	char activeParticleString[6];
	_itoa(activeParticle, activeParticleString, 10);
	strcat(particleString, activeParticleString);
	strcat(particleString, " of 1000");

	glColor3f(1.f, 1.f, 1.f);
	glRasterPos2f(-0.98f, 0.85f);
	glutBitmapString(GLUT_BITMAP_8_BY_13, particleString);
}

void displayCrescentMoon(float x, float y, float step, float scale, float fullness) {

	glColor4f(0.99f, 0.98f, 0.84f, 0.7f);
	glLineWidth(2.5);
	glBegin(GL_LINE_STRIP);
	{
		glVertex2f(x, scale + y);
		float angle = step;
		while (angle < PI) {
			float sinAngle = sinf(angle);
			float cosAngle = cosf(angle);
			glVertex2f(scale*sinAngle + x, scale*cosAngle + y);
			glVertex2f(-fullness * scale*sinAngle + x, scale*cosAngle + y);
			angle += step;
		}
		glVertex2f(x, -scale + y);
	}
	glEnd();
}

void displayShootingStar()
{
	if (shootingStar.isActive == 1)
	{
		glBegin(GL_LINES);
		{
			glColor4f(1, 1, 1, 1);
			glVertex2f(shootingStar.darkPosition.x, shootingStar.darkPosition.y);
			glColor4f(1, 1, 1, 0);
			glVertex2f(shootingStar.lightPosition.x, shootingStar.lightPosition.y);
		}
		glEnd();
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	frameCount++;
	
	// Draw stars
	displayStar();
	displayShootingStar();

	// Scene display
	displaySunset();
	displayBackground();
	displayFloor();

	displayCrescentMoon(0.75f, 0.7f, 0.1f, 0.15f, -0.45f);

	// Draw snows behind snowman
	displaySnow(1);

	// Snowman display
	drawSnowman(snowman.position.x, snowman.position.y);

	// Draw snows infront of snowman
	displaySnow(2);

	// Diagnostics bitmap texts
	displayText("Arrow keys: Snowman Movement", -1.f, 0.95f);

	displayText("Diagnostics:", -1.f, 0.9f);

	displayAmountOfActiveParticles();

	displayText("Scene Controls:", -1.f, 0.8f);

	displayText("a: toggle shake", -0.98f, 0.75f);

	displayText("s: toggle snow", -0.98f, 0.7f);

	displayText("d: toggle wind", -0.98f, 0.65f);

	displayText("e: toggle star", -0.98f, 0.6f);

	displayText("q: quit", -0.98f, 0.55f);

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

	case 'e':
		if (isStarOn == 0) {
			isStarOn = 1;
			currentStarPosition = 0;
		}
		else isStarOn = 0;
		break;

	case 'q':
		exit(0);
		break;

	case KEY_EXIT:
		exit(0);
		break;
	}
}

// Arrow keys for moving snowman
void keyReleased(unsigned char key, int x, int y)
{
	switch (key) {
	case KEY_ARROW_UP:
		arrowUpOn = 0;
		break;
	case KEY_ARROW_DOWN:
		arrowDownOn = 0;
		break;
	case KEY_ARROW_LEFT:
		arrowLeftOn = 0;
		break;
	case KEY_ARROW_RIGHT:
		arrowRightOn = 0;
		break;
	}
}

// Arrow keys for moving snowman
void specialKeyPressed(unsigned char key, int x, int y)
{
	switch (key) {
	case KEY_ARROW_UP:
		arrowUpOn = 1;
		break;
	case KEY_ARROW_DOWN:
		arrowDownOn = 1;
		break;
	case KEY_ARROW_LEFT:
		arrowLeftOn = 1;
		break;
	case KEY_ARROW_RIGHT:
		arrowRightOn = 1;
		break;
	}
}


void idle(void)
{
	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) -
		frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}
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

void initialiseStar(Particle_S * star)
{
	for (unsigned int i = 0; i < MAX_STAR_PARTICLES; i++)
	{
		star->isActive = 0;
		star->position.x = RandomFloat(-0.9f, 0.9f);
		star->position.y = RandomFloat(0.f, 0.9f);
		star->initialScale = RandomFloat(0.03f, 0.04f);
		star->scale = star->initialScale;
		star->theta = 0.0f;
		star->velocity = star->initialScale * 20;
		star->transparency = 0.1f;
		star->lineSize = star->scale * 30;
		star->scaleDirection = 1;
		star->transparencyDirection = 1;
	}
}

void initialiseSnowman()
{
	snowman.position.x = 0.f;
	snowman.position.y = -1.f;
	snowman.scale = 1.f;
}

void initialiseShootingStar()
{
	shootingStar.darkPosition.x = RandomFloat(-0.8f, 0.8f);
	shootingStar.darkPosition.y = 1.f;

	shootingStar.lightPosition.x = shootingStar.darkPosition.x + RandomFloat(0.f, 0.2f);
	shootingStar.lightPosition.y = shootingStar.darkPosition.y + RandomFloat(0.1f, 0.2f);

	shootingStar.lineWidth = 2.f;
	shootingStar.velocity = RandomFloat(0.002f, 0.004f);

	shootingStar.isActive = 1;
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
	for (unsigned int i = 0; i < MAX_SNOW_PARTICLES; i++)
	{
		initialiseSnow(&snowSystem[i]);
	}

	// star init
	for (unsigned int i = 0; i < MAX_STAR_PARTICLES; i++)
	{
		initialiseStar(&starSystem[i]);
	}

	// ground init
	groundPosition.x = RandomFloat(0.6f, 0.9f); //0.8f, -0.6f
	groundPosition.y = RandomFloat(-0.6f, -0.8f);

	// shooting star init
	initialiseShootingStar();

	// snowman init
	initialiseSnowman();
}
/******************************************************************************
* Animation functions for think function
 ******************************************************************************/

// postion=1 will spawn behind, position=2 will spawn in front of snowman
void spawnSnow(int position)
{
	// random snow amount between 0 and 3 will spawn
	int snowAmount = (int) RandomFloat(0, 3);

	int snowSpawnCounter = 0;

	// Activate 0 to 8 particles
	while (snowSpawnCounter < snowAmount && currentSnowPosition < MAX_SNOW_PARTICLES) {
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

void moveSnowmanUp()
{
	if (snowman.position.y > groundPosition.y - (1 - snowman.scale) - 0.005f) return;

	snowman.scale *= 0.95f;
	snowman.position.y += 0.01f;
}

void moveSnowmanDown()
{
	if (snowman.position.y < -1) return;

	snowman.scale /= 0.95f;
	snowman.position.y -= 0.01f;
}

void moveSnowmanLeft()
{
	if (snowman.position.x < -1 || snowman.position.x < -groundPosition.x + 0.1f) return;

	snowman.position.x -= 0.05f;
}

void moveSnowmanRight()
{
	if (snowman.position.x > 1 || snowman.position.x > groundPosition.x - 0.1f) return;

	snowman.position.x += 0.05f;
}

void starEffect(Particle_S *star)
{

	// Set flag for scale
	if (star->scale > star->initialScale * 1.15f)
		star->scaleDirection = 1;
	else if (star->scale < star->initialScale * 0.85f)
		star->scaleDirection = 2;

	// Set flag for transparency
	if (star->transparency > 1.f)
		star->transparencyDirection = 1;
	else if (star->transparency <= 0.1f)
		star->transparencyDirection = 2;
		
	// when star is on do effect
	if (star->isActive == 1)
	{
		// reset position when transparency is below 0.1
		if (star->transparency <= 0.1f)
		{
			star->position.x = RandomFloat(-0.9f, 0.9f);
			star->position.y = RandomFloat(0.f, 0.9f);

			// turn off star if off
			if (isStarOn == 0)
			{
				star->isActive = 0;
			}
		}

		// rotation
		star->theta += star->velocity;

		if (star->theta >= 360.0)
			star->theta -= 360.0;

		// Scale change
		if (star->scaleDirection == 1)
		{
			star->scale /= 1.0025f;
		}
		else if (star->scaleDirection == 2)
		{
			star->scale *= 1.0025f;
		}

		// Tranparency change
		if (star->transparencyDirection == 1)
		{
			star->transparency *= 0.975f;
		}
		else if (star->transparencyDirection == 2)
		{
			star->transparency /= 0.975f;
		}
	}
}

void spawnStar()
{
	// return out when at last index of starSystem
	if (currentStarPosition >= MAX_STAR_PARTICLES) return;

	starSystem[currentStarPosition].isActive = 1;
	currentStarPosition++;
}

void moveShootingStar()
{
	if (shootingStar.isActive == 1)
	{
		float m = ((shootingStar.lightPosition.y - shootingStar.darkPosition.y) / (shootingStar.lightPosition.x - shootingStar.darkPosition.x));
		float c = shootingStar.darkPosition.y - (m * shootingStar.darkPosition.x);

		Vec2f initialDarkPosition = { shootingStar.darkPosition.x, shootingStar.darkPosition.y };
		Vec2f initialLightPosition = { shootingStar.lightPosition.x, shootingStar.lightPosition.y };
		 
		// c = y - mx
		// y = mx + c
		// x = (y - c) / m
		shootingStar.darkPosition.x = ((initialDarkPosition.y - shootingStar.velocity) - c) / m;
		shootingStar.darkPosition.y = (m * (initialDarkPosition.x - shootingStar.velocity)) + c;

		shootingStar.lightPosition.x = ((initialLightPosition.y - shootingStar.velocity) - c) / m;
		shootingStar.lightPosition.y = (m * (initialLightPosition.x - shootingStar.velocity)) + c;
	}

	// if shooting star is out of the window
	if (shootingStar.lightPosition.x < -1.f || shootingStar.lightPosition.y < -1.f)
	{
		shootingStar.darkPosition.x = RandomFloat(-0.8f, 0.8f);
		shootingStar.darkPosition.y = 1.f;

		shootingStar.lightPosition.x = shootingStar.darkPosition.x + RandomFloat(0.f, 0.2f);
		shootingStar.lightPosition.y = shootingStar.darkPosition.y + RandomFloat(0.1f, 0.2f);

		shootingStar.velocity = RandomFloat(0.002f, 0.004f);

		shootingStar.isActive = 0;
	}
}

void think(void)
{
	// Snow particle system

	// Spawn particles every 10 frame
	if (frameCount % 10 == 0 && isParticlesOn == 1)
	{
		spawnSnow(1);
		spawnSnow(2);
	}
	

	// Spawn each stars with 50 frames delay
	if (frameCount % 50 == 0 && isStarOn == 1)
	{
		spawnStar();
	}

	// Activate shooting star every 500 frames
	if (frameCount % 500 == 0)
	{
		if(shootingStar.isActive == 0)
		shootingStar.isActive = 1;
	}

	for (unsigned int i = 0; i < MAX_SNOW_PARTICLES; i++)
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

	// Spining and breath in and out effect on the star
	for (unsigned int i = 0; i < MAX_STAR_PARTICLES; i++)
	{
		starEffect(&starSystem[i]);
	}

	moveShootingStar();

	// Snowman movement system
	if (arrowUpOn == 1) moveSnowmanUp();
	if (arrowDownOn == 1) moveSnowmanDown();
	if (arrowLeftOn == 1) moveSnowmanLeft();
	if (arrowRightOn == 1) moveSnowmanRight();
}
/******************************************************************************/