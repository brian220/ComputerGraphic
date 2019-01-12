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

GLuint programComet;
GLuint programAtm;
GLuint programEarth;
GLuint programExplosion;
GLuint vaoName[4];
GLuint vboName[4];
GLuint text[5];

bool start;

void textureInit();
void init();
void initComet();
void initAtm();
void initEarth();
void initExplosion();
void initParticlesPosition();
void initCometPosition();
VertexAttribute *drawEarth(GLfloat radius, int slice, int stack);


void display();
void lightingRotate();
void cometShading();
void moveCometVertex();
void atmShading();
void earthShading();
void explosionShading();
void moveVertex();

void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);

ParticleAttribute* cometParticles = new ParticleAttribute[1000];
ParticleAttribute* particles = new ParticleAttribute[20000];


//state 0 : earth rotate //state 1 : earth explotion
int currentState = 0;

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
	//without mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth3, iHeight3, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage3));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	//enable earth_clouds
	glEnable(GL_TEXTURE_2D);
	FIBITMAP* pIimage4 = FreeImage_Load(FreeImage_GetFileType("earth_clouds.jpg", 0), "earth_clouds.jpg");
	FIBITMAP* p32BitsImage4 = FreeImage_ConvertTo32Bits(pIimage4);
	int iWidth4 = FreeImage_GetWidth(p32BitsImage4);
	int iHeight4 = FreeImage_GetHeight(p32BitsImage4);
	glBindTexture(GL_TEXTURE_2D, text[4]);
	cout << iWidth4 << endl;
	//without mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth4, iHeight4, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage4));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void init() {
  glGenVertexArrays(4, vaoName);
  glGenBuffers(4, vboName);
  initComet();
  initAtm();
  initEarth();
  initExplosion();
}

void initComet() {
	GLuint vert = createShader("Shaders/comet.vert", "vertex");
	GLuint geom = createShader("Shaders/comet.geom", "geometry");
	GLuint frag = createShader("Shaders/comet.frag", "fragment");
	programComet = createProgram(vert, geom, frag);
	initCometPosition();
	glBindVertexArray(vaoName[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vboName[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 1000, cometParticles, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, life)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void initCometPosition() {
	for (int i = 0; i < 1000; i++) {
		cometParticles[i].position[0] = 5.0;
		cometParticles[i].position[1] = 0.0;
		cometParticles[i].position[2] = 0.0;
		cometParticles[i].speed[0] = GLfloat((rand() % 50) + 50.0f)*10.0f;
		cometParticles[i].speed[1] = GLfloat((rand() % 50) - 25.0f)*10.0f;
		cometParticles[i].speed[2] = GLfloat((rand() % 50) - 25.0f)*10.0f;
		cometParticles[i].life = 1.0f;
		cometParticles[i].fade = GLfloat(rand() % 100) / 1000.0f + 0.003f;
	}
}

void initAtm() {
	GLuint vert = createShader("Shaders/atm.vert", "vertex");
	GLuint geom = createShader("Shaders/atm.geom", "geometry");
	GLuint frag = createShader("Shaders/atm.frag", "fragment");
	programAtm = createProgram(vert, geom, frag);

	glBindVertexArray(vaoName[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vboName[1]);
	VertexAttribute *vertices;
	vertices = drawEarth(1.5, 360, 180);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * 130320, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void initEarth() {
  GLuint vert = createShader("Shaders/example.vert", "vertex");
  GLuint geom = createShader("Shaders/example.geom", "geometry");
  GLuint frag = createShader("Shaders/example.frag", "fragment");
  programEarth = createProgram(vert, geom, frag);

  glBindVertexArray(vaoName[2]);
  glBindBuffer(GL_ARRAY_BUFFER, vboName[2]);
  VertexAttribute *vertices;
  vertices = drawEarth(1.0, 360, 180);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * 130320, vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
};

VertexAttribute* drawEarth(GLfloat radius, int slice, int stack) {

	VertexAttribute *vertices;
	vertices = new VertexAttribute[130320];
	int cur = 0;

	GLfloat x, y, z;
	double slice_step = 2 * PI / slice, stack_step = PI / stack;
	for (int i = 0; i < slice; i++) {
		for (int j = 0; j < stack + 1; j++) {
			x = radius * sin(j * stack_step) * cos(i*slice_step);
			y = radius * cos(j * stack_step);
			z = radius * sin(j * stack_step) * sin(i*slice_step);
			glNormal3d(x, y, z);
			glVertex3d(x, y, z);
			vertices[cur].position[0] = x;
			vertices[cur].position[1] = y;
			vertices[cur].position[2] = z;
			vertices[cur].normal[0] = x;
			vertices[cur].normal[1] = y;
			vertices[cur].normal[2] = z;
			vertices[cur].texcoord[0] = 1 - (GLfloat)i / (GLfloat)slice;
			vertices[cur].texcoord[1] = 1 - (GLfloat)j / (GLfloat)stack;
			cur++;

			x = radius * sin(j * stack_step) * cos((i + 1)*slice_step);
			y = radius * cos(j * stack_step);
			z = radius * sin(j * stack_step) * sin((i + 1)*slice_step);
			glNormal3d(x, y, z);
			glVertex3d(x, y, z);
			vertices[cur].position[0] = x;
			vertices[cur].position[1] = y;
			vertices[cur].position[2] = z;
			vertices[cur].normal[0] = x;
			vertices[cur].normal[1] = y;
			vertices[cur].normal[2] = z;
			vertices[cur].texcoord[0] = 1 - ((GLfloat)i + 1.0) / (GLfloat)slice;
			vertices[cur].texcoord[1] = 1 - (GLfloat)j / (GLfloat)stack;
			cur++;
		}
	}
	return vertices;
}

void initExplosion() {
	GLuint vert = createShader("Shaders/particle.vert", "vertex");
	GLuint geom = createShader("Shaders/particle.geom", "geometry");
	GLuint frag = createShader("Shaders/particle.frag", "fragment");
	programExplosion = createProgram(vert, geom, frag);

	glBindVertexArray(vaoName[3]);
	glBindBuffer(GL_ARRAY_BUFFER, vboName[3]);
	initParticlesPosition();
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 20000, particles, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, life)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void initParticlesPosition() {
	for (int i = 0; i < 20000; i++) {
		particles[i].position[0] = 0.0;
		particles[i].position[1] = 0.0;
		particles[i].position[2] = 0.0;
		float stepi = 2 * PI / 180;
		int randomi = rand() % 180;
		float stepj = 2 * PI / 180;
		int randomj = rand() % 180;
		particles[i].speed[0] = sin(randomj * stepj) * cos(randomi * stepi) * 100.0;
		particles[i].speed[1] = cos(randomj * stepj) * 100.0;
		particles[i].speed[2] = sin(randomj * stepj) * sin(randomi * stepi) * 100.0;
		if (i < 5000) {
		  particles[i].speed[0] = sin(randomj * stepj) * cos(randomi * stepi) * 500.0;
		}
		particles[i].life = 2.0f;
		particles[i].fade = GLfloat(rand() % 100) / 1000.0f + 0.003f;
	}
}

void display()
{
	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_CULL_FACE);
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

	lightingRotate();

	if (currentState == 0) {
		cometShading();
		atmShading();
		
	}

	earthShading();
	if (currentState == 1) {
		explosionShading();
	}
	glutSwapBuffers();
}

void lightingRotate() {
	glPushMatrix();
	glRotatef(lightRevolutionAngle, 0, 1, 0);
	GLfloat lightpos[4] = { -3.0 - 0.0, 0.0 - 0.0,0.0 - 0.0, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	glPopMatrix();
}

int pause = 0;
void cometShading() {
	glUseProgram(programComet);
	glBindVertexArray(vaoName[0]);

	moveCometVertex();
	
	GLfloat pmtx[16];
	GLfloat mmtx[16];
	GLfloat light[4];

	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);

	GLint pmatLoc = glGetUniformLocation(programComet, "Projection");
	GLint mmatLoc = glGetUniformLocation(programComet, "ModelView");
	GLint lifeLoc = glGetUniformLocation(programComet, "Life");

	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the rotation matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	glDrawArrays(GL_POINTS, 0, 1000);

	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(0);

	glPopMatrix();
}

float cometX = 5.0;
void moveCometVertex() {
	cometX -= 0.001;
	if (cometX <= 0.5) {
		currentState = 1;
	}
	for (int i = 0; i < 1000; i++) {
		cometParticles[i].position[0] = cometParticles[i].position[0] + cometParticles[i].speed[0] / 10000.0;
		cometParticles[i].position[1] = cometParticles[i].position[1] + cometParticles[i].speed[1] / 10000.0;
		cometParticles[i].position[2] = cometParticles[i].position[2] + cometParticles[i].speed[2] / 10000.0;

		cometParticles[i].life -= cometParticles[i].fade;
		if (cometParticles[i].life < 0.0) {
			cometParticles[i].life = 1.0f;
			cometParticles[i].fade = float(rand() % 100) / 1000.0f + 0.003f;
			cometParticles[i].position[0] = cometX;
			cometParticles[i].position[1] = 0.0f;
			cometParticles[i].position[2] = 0.0f;
		}

	}
	glBindBuffer(GL_ARRAY_BUFFER, vboName[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 1000, cometParticles, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void atmShading() {

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glUseProgram(programAtm);
	glBindVertexArray(vaoName[1]);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	GLfloat pmtx[16];
	GLfloat mmtx[16];
	
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);

	GLint pmatLoc = glGetUniformLocation(programAtm, "Projection");
	GLint mmatLoc = glGetUniformLocation(programAtm, "ModelView");
	GLint cloudLoc = glGetUniformLocation(programAtm, "CloudTexture");

	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the rotation matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, text[4]);
	glUniform1i(cloudLoc, 3);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 130320);

	//glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(0);

	glPopMatrix();
}

float shrinkEarthSize = 1.0;
void earthShading() {

  glPushMatrix();
  //earth tilt
  glRotatef(23.5, 1.0f, 0.0f, 0.0f);
  //earth rotation
  glRotatef(earthRotationAngle, 0.0f, 1.0f, 0.0f);
  
  glUseProgram(programEarth);
  glBindVertexArray(vaoName[2]);
 
  GLfloat pmtx[16];
  GLfloat mmtx[16];
  GLfloat light[4];
  
  glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
  glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
  glGetLightfv(GL_LIGHT0, GL_POSITION, light);
  
  GLint pmatLoc = glGetUniformLocation(programEarth, "Projection");
  GLint mmatLoc = glGetUniformLocation(programEarth, "ModelView");
  GLint texLoc = glGetUniformLocation(programEarth, "Texture");
  GLint nMapLoc = glGetUniformLocation(programEarth, "NormalMap");
  GLint sMapLoc = glGetUniformLocation(programEarth, "SpecularMap");
  GLint LLoc = glGetUniformLocation(programEarth, "Light");
  
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
  
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 130320);
  
  glBindTexture(GL_TEXTURE_2D, NULL);
  glUseProgram(0);
  
  glPopMatrix();
}

void explosionShading() {
	
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glUseProgram(programExplosion);
	glBindVertexArray(vaoName[3]);

	moveVertex();

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	GLfloat light[4];

	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);

	GLint pmatLoc = glGetUniformLocation(programExplosion, "Projection");
	GLint mmatLoc = glGetUniformLocation(programExplosion, "ModelView");

	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the rotation matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	glDrawArrays(GL_POINTS, 0, 20000);

	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);

	glPopMatrix();
	
}

void moveVertex() {
	for (int i = 0; i < 20000; i++) {
			particles[i].position[0] = particles[i].position[0] + particles[i].speed[0] / 10000.0;
			particles[i].position[1] = particles[i].position[1] + particles[i].speed[1] / 10000.0;
			particles[i].position[2] = particles[i].position[2] + particles[i].speed[2] / 10000.0;

		particles[i].life -= particles[i].fade;
		if (particles[i].life < 0.0) {
			particles[i].life = 1.0f;
			particles[i].fade = float(rand() % 100) / 1000.0f + 0.003f;
			particles[i].position[0] = 0.0;
			particles[i].position[1] = 0.0;
			particles[i].position[2] = 0.0;
		}

	}
	glBindBuffer(GL_ARRAY_BUFFER, vboName[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 20000, particles, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
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
	case 'E':
	case 'e':
		currentState = 1;
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
