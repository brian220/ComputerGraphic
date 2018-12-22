#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <iostream>
#include <string>

using namespace std;

const float PI = 3.141592f;
float X = 0;

struct VertexAttribute
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};

int windowSize[2] = { 600, 600 };

GLuint program;
GLuint vboName;
GLuint text[4];

bool start;

VertexAttribute *drawEarth();
VertexAttribute *drawPlane();
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void init();


void textureInit()
{
	//enable 2D texture
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
}

void lighting()
{   
	glPushMatrix();
	glRotatef(X / 10, 0.0, 1.0, 0.0);
	// enable lighting
	glEnable(GL_LIGHTING);
	GLfloat diffuseColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambientColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { 3.0f, 0.0f, 0.0f, 1.0f };
	glEnable(GL_LIGHT0);								//open light0
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);	//set diffuse color of light0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);	//set ambient color of light0
	glLightfv(GL_LIGHT0, GL_POSITION, position);		//set position of light 0
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
	glutIdleFunc(idle);
	glutMainLoop();

	return 0;
}

void init() {
	GLuint vert = createShader("Shaders/example.vert", "vertex");
	GLuint geom = createShader("Shaders/example.geom", "geometry");
	GLuint frag = createShader("Shaders/example.frag", "fragment");
	program = createProgram(vert, geom, frag);
	lighting();

	//program = createProgram(frag);
	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

	VertexAttribute *vertices;
	vertices = drawEarth();
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * 130320, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

	

	//earth tilt
	glRotatef(23.5, 1.0f, 0.0f, 0.0f);
	//earth rotation
	glRotatef(X, 0.0f, 1.0f, 0.0f);

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	
	GLfloat mdl[16];
	GLdouble matModelView[16];
	GLdouble matProjection[16];
	GLdouble camera[3];
	int viewport[4];
	GLfloat light[4];
	
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	glGetLightfv(GL_LIGHT0, GL_POSITION, light);

	//cout << camera[0] << camera[1] << camera[2];
 	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");
	GLint texLoc = glGetUniformLocation(program, "Texture");
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

	//input the light position into fragment shader
	glUniform4fv(LLoc, 1, light);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 130320);
	
	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(0);
	
	glPopMatrix();
	glutSwapBuffers();

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




VertexAttribute *drawPlane() {
	VertexAttribute *vertices;
	vertices = new VertexAttribute[6];
	vertices[0].position[0] = 0.0f;	vertices[0].position[1] = 0.5f;	vertices[0].position[2] = 0.0f;
	vertices[1].position[0] = 1.0f;	vertices[1].position[1] = 0.5f;	vertices[1].position[2] = 0.0f;
	vertices[2].position[0] = 0.0f;	vertices[2].position[1] = 0.5f;	vertices[2].position[2] = 1.0f;
	vertices[3].position[0] = 1.0f;	vertices[3].position[1] = 0.5f;	vertices[3].position[2] = 0.0f;
	vertices[4].position[0] = 1.0f;	vertices[4].position[1] = 0.5f;	vertices[4].position[2] = 1.0f;
	vertices[5].position[0] = 0.0f;	vertices[5].position[1] = 0.5f;	vertices[5].position[2] = 1.0f;

	vertices[0].texcoord[0] = 0.3f;
	vertices[0].texcoord[1] = 0.5f;
	vertices[1].texcoord[0] = 0.3f;
	vertices[1].texcoord[1] = 0.5f;
	vertices[2].texcoord[0] = 0.3f;
	vertices[2].texcoord[1] = 0.5f;
	vertices[3].texcoord[0] = 0.3f;
	vertices[3].texcoord[1] = 0.5f;
	vertices[4].texcoord[0] = 0.3f;
	vertices[4].texcoord[1] = 0.5f;
	vertices[5].texcoord[0] = 0.3f;
	vertices[5].texcoord[1] = 0.5f;

	//vertices[0].color[0] = 1.0f;	vertices[0].color[1] = 0.0f;	vertices[0].color[2] = 0.0f;
	//vertices[1].color[0] = 0.0f;	vertices[1].color[1] = 1.0f;	vertices[1].color[2] = 0.0f;
	//vertices[2].color[0] = 0.0f;	vertices[2].color[1] = 0.0f;	vertices[2].color[2] = 1.0f;
	//vertices[3].color[0] = 0.0f;	vertices[3].color[1] = 1.0f;	vertices[3].color[2] = 0.0f;
	//vertices[4].color[0] = 1.0f;	vertices[4].color[1] = 1.0f;	vertices[4].color[2] = 1.0f;
	//vertices[5].color[0] = 0.0f;	vertices[5].color[1] = 0.0f;	vertices[5].color[2] = 1.0f;
	return vertices;
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

int pause = 0;
void idle() {
	if (pause != 1) {
		X = X + 0.1;
		if (X >= 360) {
			X = 0;
		}
	}
	glutPostRedisplay();
}
