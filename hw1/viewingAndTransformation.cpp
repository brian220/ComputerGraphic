#include "glut.h"
#include <iostream>
#include "math.h"
using namespace std;

void display();
void loadModelViewMatrix();
void loadProjectionMatrix();
void loadViewPortMatrix();
void drawSun();
void drawEarth();
void drawMoon();
void drawRotationAxis();
void drawPlanet(GLfloat radius, int slice, int stack);
void printTriangleVertex(GLfloat radius, GLfloat angle1, GLfloat angle2);
void reshape(int _width, int _height);
void keyboard(unsigned char key, int x, int y);
void idle();

const float PI = 3.141592f;
int width = 400, height = 400;
int X = 1; //degree
GLfloat Y = 0.5; //Radius
GLfloat red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
int pause = 0;
int pressO = 0;
int earthSlice = 360, earthStack = 180;

void lighting()
{
	// enable lighting
	glEnable(GL_LIGHTING);
	//Add directed light
	GLfloat diffuseColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambientColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glEnable(GL_LIGHT0);								//open light0
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);	//set diffuse color of light0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);	//set ambient color of light0
	glLightfv(GL_LIGHT0, GL_POSITION, position);		//set position of light 0
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("WindowName");

	lighting();
	glutDisplayFunc(display);
	
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void display()
{   
	loadModelViewMatrix();
	loadProjectionMatrix();
	loadViewPortMatrix();

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_CULL_FACE);		//remove back face
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);		//normalized normal 
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//sun
	glPushMatrix();
    drawSun();
	glPopMatrix();

	//earth
	glPushMatrix();
	drawEarth();
	glPopMatrix();

	//moon
	glPushMatrix();
	drawMoon();
	glPopMatrix();

	glPopMatrix();
	glutSwapBuffers();
}

void loadModelViewMatrix() {
	//ModelView Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void loadProjectionMatrix() {
	//Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, width / (GLfloat)height, 0.1, 1000);
}

void loadViewPortMatrix() {
	//Viewport Matrix
	glViewport(0, 0, width, height);
}

void drawSun() {
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
	drawPlanet(6.0 * Y, 240, 60);
}

void drawEarth() {
	glRotatef((X * 365)/365 , 0.0f, 1.0f, 0.0f);
	glTranslatef(20.0, 0.0, 0.0);
	glPushMatrix();
	glRotatef(23.5, 1.0f, 0.0f, 0.0f);
	glRotatef(X * 365, 0.0f, 1.0f, 0.0f);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
	drawPlanet(2 * Y, earthSlice, earthStack);
	glRotatef(90, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
	drawRotationAxis();
}

void drawMoon() {
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
	glRotatef((X * 365) / 28, 0.0f, 1.0f, 0.0f);
	glTranslatef(3.0, 0.0, 0.0);
	glRotatef((X * 365) / 28, 0.0f, 1.0f, 0.0f);
	drawPlanet(Y, 240, 60);
}

void drawRotationAxis() {
	GLUquadricObj *quad = gluNewQuadric();
	gluCylinder(quad, 0.2, 0.2, 4.0 * Y, 5, 5);
	glRotatef(180, 1.0f, 0.0f, 0.0f);
	gluCylinder(quad, 0.2, 0.2, 4.0 * Y, 5, 5);
}

void drawPlanet(GLfloat radius, int slice, int stack) {
	GLfloat hAngle, vAngle;
	GLfloat horizontalStep = 2 * PI / GLfloat(slice);
	GLfloat verticalStep = PI / GLfloat(stack);
	for (hAngle = 0.0; hAngle < 2.01 * PI; hAngle += horizontalStep) {
		glBegin(GL_TRIANGLE_STRIP);
		for (vAngle = 0.0; vAngle < 2.01 * PI; vAngle += verticalStep) {
			printTriangleVertex(radius, hAngle, vAngle);
			printTriangleVertex(radius, hAngle + horizontalStep, vAngle);
		}
		glEnd();
	}
}

void printTriangleVertex(GLfloat radius, GLfloat angle1, GLfloat angle2) {
	GLfloat x = radius * cos(angle2) * sin(angle1);
	GLfloat y = radius * sin(angle2) * sin(angle1);
	GLfloat z = radius * cos(angle1);
	glNormal3f(x / radius, y / radius, z / radius);
	glVertex3f(x, y, z);
}

void reshape(int _width, int _height) {
	width = _width;
	height = _height;
}


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'p':
	case 'P':
		if (pause == 0) {
			pause = 1;
		}
		else {
			pause = 0;
		}
		break;
	case 'o':
	case 'O':
		if(pressO == 0) {
			earthSlice = 4;
			earthStack = 2;
			pressO = 1;
		}
		else if (pressO == 1) {
			earthSlice = 360;
			earthStack = 180;
			pressO = 0;
		}
	default:
		break;
	}
}

void idle() {
	if (pause != 1) {
		X = int(X + 1) % 360;
	}
	glutPostRedisplay();
}