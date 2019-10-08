#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <stdlib.h>
#include <time.h>   
#include <iostream>
#include <string>
#include <vector>
#define VERTEXNUMBER 130320
#define PLANETNUMBER 2
#define TEXTURENUMBER 3

using namespace std;

class VertexAttribute {
public:
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
	void setPosition(float x, float y, float z) {
		position[0] = x;
		position[1] = y;
		position[2] = z;
	}
	void setNormal(float x, float y, float z) {
		normal[0] = x;
		normal[1] = y;
		normal[2] = z;
	}
	void setTexcoord(float u, float v) {
		texcoord[0] = u;
		texcoord[1] = v;
	}
};

const float PI = 3.141592f;
float earthRotationAngle = 0.0;
float earthRotationSpeed = 0.3;
float moonRotationAngle = 0.0;
float moonRotationSpeed = 0.3;
float moonRevolutionAngle = 0.0;
float moonRevolutionSpeed = 0.1;

float lightRevolutionAngle = 0.0;
float lightRevolutionSpeed = earthRotationSpeed / 10.0;
int windowSize[2] = { 600, 600 };
GLfloat earthRadius = 1.0;
GLfloat moonRadius = 0.5;
void lightingRotate();

//GLUT functions
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);


//For shader
GLuint programEarth;
GLuint programMoon;
GLuint vaoName[2];
GLuint vboName[2];
GLuint earthShaderIndex = 0;
GLuint moonShaderIndex = 1;
void initShader();
void generateBuffer();
void generateVertexArrays();
void createPlanetProgram(GLuint &program);
void initMoonShader();
void initEarthShader();
vector<VertexAttribute> drawPlanet(GLfloat radius);
vector<VertexAttribute>  computePlanetVerticesByRadius(GLfloat radius);
void putVerticesToVBO(int planetIndex, vector <VertexAttribute> vertices);

//For texture
GLuint text[5];
GLuint earthTextureIndex = 1;
GLuint moonTextureIndex = 2;
void initTexture();
void gernerateTexture();
void initEarthTexture();
void initMoonTexture();
void initPlanetTexture(GLuint textureIndex, char* fileName);
void planetShading(GLuint planetProgram, GLuint planetTextureIndex, GLuint planetShaderIndex);

void initShader() {
	generateBuffer();
	generateVertexArrays();
	
	createPlanetProgram(programEarth);
	createPlanetProgram(programMoon);
	
	initEarthShader();
	initMoonShader();
}

void generateBuffer() {
	glGenBuffers(2, vboName);
}

void generateVertexArrays() {
	glGenVertexArrays(2, vaoName);
}

void createPlanetProgram(GLuint &program) {
	GLuint vert = createShader("Shaders/example.vert", "vertex");
	GLuint geom = createShader("Shaders/example.geom", "geometry");
	GLuint frag = createShader("Shaders/example.frag", "fragment");
	program = createProgram(vert, geom, frag);
}

void initEarthShader() {
	vector <VertexAttribute> vertices = computePlanetVerticesByRadius(earthRadius);
	putVerticesToVBO(earthShaderIndex, vertices);
}

void initMoonShader() {
	vector <VertexAttribute> vertices = computePlanetVerticesByRadius(moonRadius);
	putVerticesToVBO(moonShaderIndex, vertices);
}

vector <VertexAttribute> computePlanetVerticesByRadius(GLfloat radius) {
	vector <VertexAttribute> vertexVector;
	vertexVector = drawPlanet(radius);
	return vertexVector;
}

vector<VertexAttribute> drawPlanet(GLfloat radius) {
	vector <VertexAttribute> vertices;
	GLfloat x, y, z;
	GLuint slice = 360;
	GLuint stack = 180;
	double slice_step = 2 * PI / slice, stack_step = PI / stack;
	for (int i = 0; i < slice; i++) {
		for (int j = 0; j < stack + 1; j++) {
			VertexAttribute currentVertex1;
			x = sin(j * stack_step) * cos(i*slice_step);
			y = cos(j * stack_step);
			z = sin(j * stack_step) * sin(i*slice_step);
			currentVertex1.setPosition(x * radius, y * radius, z * radius);
			currentVertex1.setNormal(x, y, z);
			currentVertex1.setTexcoord((1 - (GLfloat)i / (GLfloat)slice), (1 - (GLfloat)j / (GLfloat)stack));
			vertices.push_back(currentVertex1);

			VertexAttribute currentVertex2;
			x = sin(j * stack_step) * cos((i + 1)*slice_step);
			y = cos(j * stack_step);
			z = sin(j * stack_step) * sin((i + 1)*slice_step);
			currentVertex2.setPosition(x * radius, y * radius, z * radius);
			currentVertex2.setNormal(x, y, z);
			currentVertex2.setTexcoord((1 - ((GLfloat)i + 1.0) / (GLfloat)slice), (1 - (GLfloat)j / (GLfloat)stack));
			vertices.push_back(currentVertex2);
		}
	}
	return vertices;
}

void putVerticesToVBO(int planetIndex, vector <VertexAttribute> vertices) {
	glBindVertexArray(vaoName[planetIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, vboName[planetIndex]);
	VertexAttribute *v = &vertices[0];
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * VERTEXNUMBER, v, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void initTexture() {
	gernerateTexture();
	initEarthTexture();
	initMoonTexture();
}

void gernerateTexture() {
	glGenTextures(TEXTURENUMBER, text);
}

void initEarthTexture() {
	initPlanetTexture(earthTextureIndex, (char*)"earth_texture_map.jpg");
}

void initMoonTexture() {
	initPlanetTexture(moonTextureIndex, (char*)"moon.jpg");
}

void initPlanetTexture(GLuint textureIndex, char* fileName) {
	//enable earth_texture_map
	glEnable(GL_TEXTURE_2D);
	FIBITMAP* pIimage = FreeImage_Load(FreeImage_GetFileType(fileName, 0), fileName);
	FIBITMAP* p32BitsImage = FreeImage_ConvertTo32Bits(pIimage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);
	glBindTexture(GL_TEXTURE_2D, text[textureIndex]);

	//without mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void display()
{
	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);
	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//lights();
	gluLookAt(0.0f, 0.0f, 15.0f,// eye
		0.0f, 0.0f, 0.0f,// center
		0.0f, 1.0f, 0.0f);// up

	//light
	lightingRotate();

	//earth tilt
	glRotatef(23.5, 1.0f, 0.0f, 0.0f);
	//earth rotation
	glRotatef(earthRotationAngle, 0.0f, 1.0f, 0.0f);
	//Draw Moon by shader
	planetShading(programEarth, earthTextureIndex, earthShaderIndex);
	glPopMatrix();
	glRotatef(moonRevolutionAngle, 0.0f, 1.0f, 0.0f);
	glTranslatef(5.0f, 0.0f, 0.0f);
	glRotatef(moonRotationAngle, 0.0f, 1.0f, 0.0f);
	//Draw Earth by shader
	planetShading(programMoon, moonTextureIndex, moonShaderIndex);
	glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
}

void lightingRotate() {
	glPushMatrix();
	glRotatef(lightRevolutionAngle, 0, 1, 0);
	GLfloat lightpos[4] = { -3.0 - 0.0, 0.0 - 0.0,0.0 - 0.0, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	glPopMatrix();
}

void planetShading(GLuint planetProgram, GLuint textureIndex, GLuint shaderIndex) {
	glUseProgram(planetProgram);
	glBindVertexArray(vaoName[shaderIndex]);

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	GLfloat light[4];

	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	glGetLightfv(GL_LIGHT0, GL_POSITION, light);

	GLint pmatLoc = glGetUniformLocation(programEarth, "Projection");
	GLint mmatLoc = glGetUniformLocation(programEarth, "ModelView");
	GLint texLoc = glGetUniformLocation(programEarth, "Texture");

	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the rotation matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	//append the texture into the fragment shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text[textureIndex]);
	glUniform1i(texLoc, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, VERTEXNUMBER);
	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(0);
	glPopMatrix();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

int pause = 0;
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
	}
}

void idle() {
		earthRotationAngle = earthRotationAngle + earthRotationSpeed;
		if (earthRotationAngle >= 360) {
			earthRotationAngle = 0;
		}
		moonRotationAngle = moonRotationAngle + moonRotationSpeed;
		if (moonRotationAngle >= 360) {
			moonRotationAngle = 0;
		}
		moonRevolutionAngle = moonRevolutionAngle + moonRevolutionAngle;
		if (moonRevolutionAngle >= 360) {
			moonRevolutionAngle = 0;
		}
		lightRevolutionAngle = lightRevolutionAngle + lightRevolutionSpeed;
		if (lightRevolutionAngle >= 360) {
			lightRevolutionAngle = 0;
		}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("ComputerGraphicsDemo");
	
	lightingRotate();
	glewInit();
	initTexture();
	initShader();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();

	return 0;
}
