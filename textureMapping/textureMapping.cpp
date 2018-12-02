#include "glew.h"
#include "glut.h"
#include <Windows.h>
#include <iostream>
#include "math.h"
#include "FreeImage.h"
#pragma comment(lib,"FreeImage.lib")
using namespace std;

void display();
void loadModelViewMatrix();
void loadProjectionMatrix();
void loadViewPortMatrix();
void drawEarth();
void drawMoon();
void drawPlanet(GLfloat radius, int slice, int stack, int planetTex);
void reshape(int _width, int _height);
void keyboard(unsigned char key, int x, int y);
void idle();

const float PI = 3.141592f;
int width = 400, height = 400;
float X = 1; //degree
GLfloat Y = 3; //Radius
GLfloat red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
int pause = 0;
int pressO = 0;
int earthSlice = 360, earthStack = 180;
GLuint text[10];


void textureInit()
{
	//enable 2D texture
	glEnable(GL_TEXTURE_2D);
	FIBITMAP* pIimage = FreeImage_Load(FreeImage_GetFileType("earth.jpg", 0), "earth.jpg");
	FIBITMAP* p32BitsImage = FreeImage_ConvertTo32Bits(pIimage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);
	glGenTextures(10, text);
	glBindTexture(GL_TEXTURE_2D, text[1]);

	//without mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	//enable 2D texture
	glEnable(GL_TEXTURE_2D);
	FIBITMAP* pIimage2 = FreeImage_Load(FreeImage_GetFileType("moon.jpg", 0), "moon.jpg");
	FIBITMAP*  p32BitsImage2 = FreeImage_ConvertTo32Bits(pIimage2);
    int iWidth2 = FreeImage_GetWidth(p32BitsImage2);
	int iHeight2 = FreeImage_GetHeight(p32BitsImage2);
	//glGenTextures(1, text);
	glBindTexture(GL_TEXTURE_2D, text[2]);

	//without mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage2));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

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
	glewInit();
	lighting();
	textureInit();
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//earth
	glPushMatrix();
    drawEarth();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);
	//moon
	glPushMatrix();
	drawMoon();
	glPopMatrix();

	glutSwapBuffers();
}

void loadModelViewMatrix() {
	//ModelView Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 50.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
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

void drawEarth() {
	glRotatef(23.5, 0.0f, 0.0f, 1.0f);
	glRotatef(X * 365, 0.0f, 1.0f, 0.0f);
	drawPlanet(Y, earthSlice, earthStack, 1);
}

void drawMoon() {
	glRotatef(((float)X * 365) / 28.0, 0.0f, 1.0f, 0.0f);
	glTranslatef(3.0 * Y, 0.0, 0.0);
	glRotatef(((float)X * 365) / 28.0 , 0.0f, 1.0f, 0.0f);
	drawPlanet(0.5 * Y, 240, 60, 2);
}


void drawPlanet(GLfloat radius, int slice, int stack, int planetTex) {
	GLfloat hAngle, vAngle;
	GLfloat horizontalStep = 2 * PI / GLfloat(slice);
	GLfloat verticalStep = PI / GLfloat(stack);
	
	for (hAngle = 0.0; hAngle < 2.01 * PI; hAngle += horizontalStep) {
		glBindTexture(GL_TEXTURE_2D, text[planetTex]);
		glBegin(GL_TRIANGLE_STRIP);
		for (vAngle = 0.0; vAngle < 2.01 * PI; vAngle += verticalStep) {
			GLfloat x = radius * sin(vAngle) * cos(hAngle);
			GLfloat z = radius * sin(vAngle) * sin(hAngle);
			GLfloat y = radius * cos(vAngle);

			glNormal3f(x / radius, y / radius, z / radius);
			glTexCoord2f((hAngle/horizontalStep) / (GLfloat)slice, 1.0 - (vAngle / verticalStep) / (GLfloat)stack);
			glVertex3f(x, y, z);

			x = radius * sin(vAngle) * cos(hAngle + horizontalStep);
			z = radius * sin(vAngle) * sin(hAngle + horizontalStep);
			y = radius * cos(vAngle);

			glNormal3f(x / radius, y / radius, z / radius);
			glTexCoord2f(((hAngle / horizontalStep) + 1.0) / (GLfloat) slice, 1.0 - (vAngle / verticalStep) / (GLfloat)stack);
			glVertex3f(x, y, z);
		}
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	}
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