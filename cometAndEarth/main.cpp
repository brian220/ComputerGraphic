#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <stdlib.h>
#include <time.h>   
#include <iostream>
#include <string>

using namespace std;

const float PI = 3.141592f;
float earthRotationAngle = 0.0;
float earthRotationSpeed = 0.1;
float lightRevolutionAngle = 0.0;
float lightRevolutionSpeed = earthRotationSpeed / 10.0;

struct VertexAttribute
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};

struct ParticleAttribute
{
	GLfloat position[3] = {};
	GLfloat speed[3];
	//GLfloat color[3];
	GLfloat fade;
	GLfloat life;
};

int windowSize[2] = { 600, 600 };

GLuint program;
GLuint vboName;
GLuint text[4];

bool start;

VertexAttribute *drawEarth();
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);
void init();
void initParticlesPosition();
void moveVertex();
float getRandomSpeed();

ParticleAttribute* particles = new ParticleAttribute[20000];

void textureInit()
{
	//enable earth_texture_map
	glEnable(GL_TEXTURE_2D);
	FIBITMAP* pIimage = FreeImage_Load(FreeImage_GetFileType("earth_texture_map.jpg", 0), "earth_texture_map.jpg");
	FIBITMAP* p32BitsImage = FreeImage_ConvertTo32Bits(pIimage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);
	glGenTextures(3, text);
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

	
	//enable earth_normal_map
	glEnable(GL_TEXTURE_2D);
	FIBITMAP* pIimage2 = FreeImage_Load(FreeImage_GetFileType("earth_normal_map.tif", 0), "earth_normal_map.tif");
	FIBITMAP* p32BitsImage2 = FreeImage_ConvertTo32Bits(pIimage2);
	int iWidth2 = FreeImage_GetWidth(p32BitsImage2);
	int iHeight2 = FreeImage_GetHeight(p32BitsImage2);
	glBindTexture(GL_TEXTURE_2D, text[2]);
	//without mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth2, iHeight2, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage2));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	//enable earth_specular_map
	glEnable(GL_TEXTURE_2D);
	FIBITMAP* pIimage3 = FreeImage_Load(FreeImage_GetFileType("earth_specular_map.tif", 0), "earth_specular_map.tif");
	FIBITMAP* p32BitsImage3 = FreeImage_ConvertTo32Bits(pIimage3);
	int iWidth3 = FreeImage_GetWidth(p32BitsImage3);
	int iHeight3 = FreeImage_GetHeight(p32BitsImage3);
	glBindTexture(GL_TEXTURE_2D, text[3]);
	cout << iWidth << endl;
	//without mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth3, iHeight3, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage3));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void lightingRotate()
{   
	glPushMatrix();
	glRotatef(lightRevolutionAngle, 0, 1, 0);
	GLfloat lightpos[4] = { -3.0 - 0.0, 0.0 - 0.0,0.0 - 0.0, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	glPopMatrix();
}

int main(int argc, char** argv)
{	
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("ComputerGraphicsDemo");
	glewInit();
	textureInit();
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();

	return 0;
}

void init() {
	//GLuint vert = createShader("Shaders/example.vert", "vertex");
	//GLuint geom = createShader("Shaders/example.geom", "geometry");
	//GLuint frag = createShader("Shaders/example.frag", "fragment");

	GLuint vert = createShader("Shaders/particle.vert", "vertex");
	GLuint geom = createShader("Shaders/particle.geom", "geometry");
	GLuint frag = createShader("Shaders/particle.frag", "fragment");
	program = createProgram(vert, geom, frag);
	
	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);
	initParticlesPosition();
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 20000, particles, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, color)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*
	VertexAttribute *vertices;
	vertices = drawExplode();
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * 100, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	*/
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
	gluLookAt(0.0f, 0.0f, 3.0f,// eye
		0.0f, 0.0f, 0.0f,// center
		0.0f, 1.0f, 0.0f);// up

	lightingRotate();
	moveVertex();

	//earth tilt
	glRotatef(23.5, 1.0f, 0.0f, 0.0f);
	//earth rotation
	//glRotatef(earthRotationAngle, 0.0f, 1.0f, 0.0f);

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	GLfloat light[4];
	
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	glGetLightfv(GL_LIGHT0, GL_POSITION, light);

	//cout << camera[0] << camera[1] << camera[2];
 	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");
	GLint texLoc = glGetUniformLocation(program, "Texture");
	GLint nMapLoc = glGetUniformLocation(program, "NormalMap");
	GLint sMapLoc = glGetUniformLocation(program, "SpecularMap");
	GLint LLoc = glGetUniformLocation(program, "Light");

	glUseProgram(program);
	
	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the rotation matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	//append the texture into the fragment shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text[1]);
	glUniform1i(texLoc, 0);
	//append the normal map into the vertex shader
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, text[2]);
	glUniform1i(nMapLoc, 1);
	//append the texture into the fragment shader
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, text[3]);
	glUniform1i(sMapLoc, 2);

	//input the light position into fragment shader
	glUniform4fv(LLoc, 1, light);

	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 100);
	
	glDrawArrays(GL_POINTS, 0, 20000);

	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(0);
	
	glPopMatrix();
	glutSwapBuffers();
}



void initParticlesPosition() {
   for (int i = 0; i < 20000; i++) {
	   particles[i].position[0] = 0.0;
	   particles[i].position[1] = 0.0;
	   particles[i].position[2] = 0.0;
	   particles[i].speed[0] = GLfloat((rand() % 50) - 26.0f)*10.0f;
	   particles[i].speed[1] = GLfloat((rand() % 50) - 25.0f)*10.0f;
	   particles[i].speed[2] = GLfloat((rand() % 50) - 25.0f)*10.0f;
	   particles[i].life = 1.0f;
	   particles[i].fade = GLfloat(rand() % 100) / 1000.0f + 0.003f;
	   //particles[i].color[0] = 1.0;
	   //particles[i].color[1] = 1.0;
	   //particles[i].color[2] = 1.0;
   }
}

GLfloat Time = 0.0;
void moveVertex() {
	srand(time(NULL));
	Time += 0.0001;
	for (int i = 0; i < 20000; i++) {
		float speedX = getRandomSpeed();
		float speedY = getRandomSpeed();
		float speedZ = getRandomSpeed();

		particles[i].position[0] = particles[i].position[0] + particles[i].speed[0] / 10000.0;
		particles[i].position[1] = particles[i].position[1] + particles[i].speed[1] / 10000.0;
		particles[i].position[2] = particles[i].position[2] + particles[i].speed[2] / 10000.0;

		/*
		if (rand() % 3 == 0) {
			particles[i].color[0] = 1.0;
			particles[i].color[1] = 1.0;
			particles[i].color[2] = 1.0;
		}
		else if (rand() % 3 == 1) {
			particles[i].color[0] = 0.5;
			particles[i].color[1] = 0.5;
			particles[i].color[2] = 0.0;
		}
		else if (rand() % 3 == 2) {
			particles[i].color[0] = 0.8;
			particles[i].color[1] = 0.0;
			particles[i].color[2] = 0.8;
		}
	    */

		particles[i].life -= particles[i].fade;
		if (particles[i].life < 0.0) {
			particles[i].life = 1.0f;
			particles[i].fade = float(rand() % 100) / 1000.0f + 0.003f;
			particles[i].position[0] = 0.0f;
			particles[i].position[1] = 0.0f;
			particles[i].position[2] = 0.0f;
		}

	}
	glBindBuffer(GL_ARRAY_BUFFER, vboName);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 20000, particles, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, color)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

float getRandomSpeed() {
	int x = rand();
	float randomSpeed = float((rand() % 50) - 26.0f)*10.0f;
	return randomSpeed;
}

VertexAttribute* drawEarth() {

	VertexAttribute *vertices;
	vertices = new VertexAttribute[130320];
	int cur = 0;

	GLfloat radius = 1;
	int slice = 360;
	int stack = 180;
	GLfloat x, y, z;
	double slice_step = 2 * PI / slice, stack_step = PI / stack;
	for (int i = 0; i < slice; i++) {
		for (int j = 0; j < stack + 1; j++) {
			x = sin(j * stack_step) * cos(i*slice_step);
			y = cos(j * stack_step);
			z = sin(j * stack_step) * sin(i*slice_step);
			glNormal3d(x, y, z);
			glVertex3d(x, y, z);
			vertices[cur].position[0] = x;
			vertices[cur].position[1] = y;
			vertices[cur].position[2] = z;
			vertices[cur].normal[0] = x;
			vertices[cur].normal[1] = y;
			vertices[cur].normal[2] = z;
			vertices[cur].texcoord[0] = 1 - (GLfloat) i / (GLfloat)slice;
			vertices[cur].texcoord[1] = 1 - (GLfloat) j / (GLfloat)stack;
			cur++;

			x = sin(j * stack_step) * cos((i + 1)*slice_step);
			y = cos(j * stack_step);
			z = sin(j * stack_step) * sin((i + 1)*slice_step);
			glNormal3d(x, y, z);
			glVertex3d(x, y, z);
			vertices[cur].position[0] = x;
			vertices[cur].position[1] = y;
			vertices[cur].position[2] = z;
			vertices[cur].normal[0] = x;
			vertices[cur].normal[1] = y;
			vertices[cur].normal[2] = z;
			vertices[cur].texcoord[0] = 1 - ((GLfloat)i + 1.0 ) / (GLfloat)slice;
			vertices[cur].texcoord[1] = 1 - (GLfloat) j / (GLfloat)stack;
			cur++;
		}
	}
	return vertices;
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
	default:
		break;
	}
}


void idle() {
	if (pause != 1) {
		earthRotationAngle = earthRotationAngle + earthRotationSpeed;
		if (earthRotationAngle >= 360) {
			earthRotationAngle = 0;
		}
		lightRevolutionAngle = lightRevolutionAngle + lightRevolutionSpeed;
		if (lightRevolutionAngle >= 360) {
			lightRevolutionAngle = 0;
		}
	}
	glutPostRedisplay();
}
