#include "glut.h"
#include <iostream>

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

void SetLightSource() {
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f };
	GLfloat light_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("WindowName");

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
	
	SetLightSource();
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	
	//sun
	glPushMatrix();
	drawSun();
	//glutWireSphere(1, 18, 18);
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
	//glColor3f(1.0f, 0.65f, 0.0f);
	drawPlanet(6.0 * Y, 240, 60);
}

void drawEarth() {
	glRotatef((X * 365)/365 , 0.0f, 1.0f, 0.0f);
	glTranslatef(20.0, 0.0, 0.0);
	glPushMatrix();
	glRotatef(X * 365, 0.0f, 1.0f, 0.0f);
	glRotatef(23.5, 1.0f, 0.0f, 0.0f);
	drawRotationAxis();
	drawPlanet(2 * Y, 360 / 4, 180 / 2);
}

void drawMoon() {
	glRotatef((X * 365) / 28, 0.0f, 1.0f, 0.0f);
	glTranslatef(3.0, 0.0, 0.0);
	glRotatef((X * 365) / 28, 0.0f, 1.0f, 0.0f);
	glColor3f(0.5f, 0.5f, 0.5f);
	drawPlanet(Y, 240, 60);
}

void drawRotationAxis() {
	glBegin(GL_LINES);
	glVertex3f(0.0, 4*Y, 0.0);
	glVertex3f(0.0, -4*Y, 0.0);
	glEnd();
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
	glVertex3f(x, y, z);
}

void reshape(int _width, int _height) {
	width = _width;
	height = _height;
}


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	default:
		break;
	}
}

void idle() {
	X = int(X + 1) % 360;
	glutPostRedisplay();
}

