/*

CG Homework1 - Phong Shading, Gouraud Shading, Flat Shading ...

Objective - learning GLSL, glm model datatype(for .obj file) 

Overview:

	1. Render the model with texture using VBO and/or VAO

	2. Implement Phong shading, Gouraud Shading and Flat Shading

Good luck!

!!!IMPORTANT!!! 

1. Make sure to change the window name to your student ID!
2. Make sure to remove glmDraw() at line 207.
   It's meant for a quick preview of the ball object and shouldn't exist in your final code.

*/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <stddef.h> /*for function: offsetof */
#include <math.h>
#include <string.h>
#include "../GL/glew.h"
#include "../GL/glut.h"
#include "../shader_lib/shader.h"
#include "glm/glm.h"

extern "C"
{
	#include "glm_helper.h"
}

/*you may need to do something here
you may use the following struct type to perform your single VBO method,
or you can define/declare multiple VBOs for VAO method.
Please feel free to modify it*/
struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};
typedef struct Vertex Vertex;

//no need to modify the following function declarations and gloabal variables
void init(void);
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void idle(void);
void draw_light_bulb(void);
void camera_light_ball_move();
GLuint loadTexture(char* name, GLfloat width, GLfloat height);

void saveVerticesDataFromModel();
void initPhongShader();
void initGouraudShader();
void initFlatShader();

namespace
{
	char *obj_file_dir = "../Resources/Ball.obj";
	char *main_tex_dir = "../Resources/WoodFine.ppm";
	
	GLfloat light_rad = 0.05; //radius of the light bulb
	float eyet = 0.0; //theta in degree
	float eyep = 90.0; //phi in degree
	bool mleft = false;
	bool mright = false;
	bool mmiddle = false;
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool lforward = false;
	bool lbackward = false;
	bool lleft = false;
	bool lright = false;
	bool lup = false;
	bool ldown = false;
	bool bforward = false;
	bool bbackward = false;
	bool bleft = false;
	bool bright = false;
	bool bup = false;
	bool bdown = false;
	bool bx = false;
	bool by = false;
	bool bz = false;
	bool brx = false;
	bool bry = false;
	bool brz = false;

	int mousex = 0;
	int mousey = 0;
}

// You can modify the moving/rotating speed if it's too fast/slow for you
const float speed = 0.003; // camera / light / ball moving speed
const float rotation_speed = 0.5; // ball rotating speed

//you may need to use some of the following variables in your program 

// No need for model texture, 'cause glmModel() has already loaded it for you.
// To use the texture, check glmModel documentation.
GLuint mainTextureID; // TA has already loaded this texture for you

GLMmodel *model; //TA has already loaded the model for you(!but you still need to convert it to VBO(s)!)

float eyex = 0.0;
float eyey = 0.0;
float eyez = 5.6;

GLfloat light_pos[] = { 1.1, 1.0, 1.3 };
GLfloat ball_pos[] = { 0.0, 0.0, 0.0 };
GLfloat ball_rot[] = { 0.0, 10.0, 0.0 };

GLuint phongIndex = 0;
GLuint gouraudIndex = 1;
GLuint flatIndex = 2;
GLuint shaderIndex = 0;

GLuint vaoName[3];
GLuint vboName[3];
GLuint program[3];
GLuint verticeNumber;
Vertex *vertices;


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	
	// remember to replace "YourStudentID" with your own student ID
	glutCreateWindow("CG_HW1_0856601");
	glutReshapeWindow(512, 512);

	glewInit();

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();

	glmDelete(model);
	return 0;
}

void init(void)
{   
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glEnable(GL_CULL_FACE);

	model = glmReadOBJ(obj_file_dir);

	mainTextureID = loadTexture(main_tex_dir, 1024, 1024);

	glmUnitize(model);
	glmFacetNormals(model);
	glmVertexNormals(model, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(model);

	// Load the model vertices data
	saveVerticesDataFromModel();

	glGenBuffers(3, vboName);
	glGenVertexArrays(3, vaoName);
	initPhongShader();
	initGouraudShader();
	initFlatShader();
}

void saveVerticesDataFromModel() {
	GLuint triangleNumber = model->numtriangles;
	verticeNumber = triangleNumber * 3;
	vertices = new Vertex[verticeNumber];
	for (int i = 0; i < triangleNumber; i++) {
		int vindex0 = model->triangles[i].vindices[0];
		int vindex1 = model->triangles[i].vindices[1];
		int vindex2 = model->triangles[i].vindices[2];

		int nindex0 = model->triangles[i].nindices[0];
		int nindex1 = model->triangles[i].nindices[1];
		int nindex2 = model->triangles[i].nindices[2];

		int tindex0 = model->triangles[i].tindices[0];
		int tindex1 = model->triangles[i].tindices[1];
		int tindex2 = model->triangles[i].tindices[2];

		vertices[3 * i + 0].position[0] = model->vertices[3 * vindex0 + 0];
		vertices[3 * i + 0].position[1] = model->vertices[3 * vindex0 + 1];
		vertices[3 * i + 0].position[2] = model->vertices[3 * vindex0 + 2];
		vertices[3 * i + 0].normal[0] = model->normals[3 * nindex0 + 0];
		vertices[3 * i + 0].normal[1] = model->normals[3 * nindex0 + 1];
		vertices[3 * i + 0].normal[2] = model->normals[3 * nindex0 + 2];
		vertices[3 * i + 0].texcoord[0] = model->texcoords[2 * tindex0 + 0];
		vertices[3 * i + 0].texcoord[1] = model->texcoords[2 * tindex0 + 1];

		vertices[3 * i + 1].position[0] = model->vertices[3 * vindex1 + 0];
		vertices[3 * i + 1].position[1] = model->vertices[3 * vindex1 + 1];
		vertices[3 * i + 1].position[2] = model->vertices[3 * vindex1 + 2];
		vertices[3 * i + 1].normal[0] = model->normals[3 * nindex1 + 0];
		vertices[3 * i + 1].normal[1] = model->normals[3 * nindex1 + 1];
		vertices[3 * i + 1].normal[2] = model->normals[3 * nindex1 + 2];
		vertices[3 * i + 1].texcoord[0] = model->texcoords[2 * tindex1 + 0];
		vertices[3 * i + 1].texcoord[1] = model->texcoords[2 * tindex1 + 1];

		vertices[3 * i + 2].position[0] = model->vertices[3 * vindex2 + 0];
		vertices[3 * i + 2].position[1] = model->vertices[3 * vindex2 + 1];
		vertices[3 * i + 2].position[2] = model->vertices[3 * vindex2 + 2];
		vertices[3 * i + 2].normal[0] = model->normals[3 * nindex2 + 0];
		vertices[3 * i + 2].normal[1] = model->normals[3 * nindex2 + 1];
		vertices[3 * i + 2].normal[2] = model->normals[3 * nindex2 + 2];
		vertices[3 * i + 2].texcoord[0] = model->texcoords[2 * tindex2 + 0];
		vertices[3 * i + 2].texcoord[1] = model->texcoords[2 * tindex2 + 1];
	}
}

void initPhongShader() {
	GLuint vert = createShader("Shaders/phong.vert", "vertex");
	GLuint frag = createShader("Shaders/phong.frag", "fragment");
	program[phongIndex] = createProgram(vert, frag);

	//Generate a new buffer object
	glBindVertexArray(vaoName[phongIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, vboName[phongIndex]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verticeNumber, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void initGouraudShader() {
	GLuint vert = createShader("Shaders/gouraud.vert", "vertex");
	GLuint frag = createShader("Shaders/gouraud.frag", "fragment");
	program[gouraudIndex] = createProgram(vert, frag);

	//Generate a new buffer object
	glBindVertexArray(vaoName[gouraudIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, vboName[gouraudIndex]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verticeNumber, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void initFlatShader() {
	GLuint vert = createShader("Shaders/flat.vert", "vertex");
	GLuint frag = createShader("Shaders/flat.frag", "fragment");
	program[flatIndex] = createProgram(vert, frag);

	//Generate a new buffer object
	glBindVertexArray(vaoName[flatIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, vboName[flatIndex]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verticeNumber, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void display(void)
{   
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//you may need to do something here(declare some local variables you need and maybe load Model matrix here...)

	//please try not to modify the following block of code(you can but you are not supposed to)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		eyex, 
		eyey, 
		eyez,
		eyex+cos(eyet*M_PI/180)*cos(eyep*M_PI / 180), 
		eyey+sin(eyet*M_PI / 180), 
		eyez-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180),
		0.0,
		1.0,
		0.0);
	draw_light_bulb();
	glPushMatrix();
		glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);
		glRotatef(ball_rot[0], 1, 0, 0);
		glRotatef(ball_rot[1], 0, 1, 0);
		glRotatef(ball_rot[2], 0, 0, 1);
	// please try not to modify the previous block of code

	// you may need to do something here(pass uniform variable(s) to shader and render the model)
	//glmDraw(model, GLM_TEXTURE); // please delete this line in your final code! It's just a preview of rendered object
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	GLfloat light[4];

	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	glGetLightfv(GL_LIGHT0, GL_POSITION, light);
	GLint pmatLoc = glGetUniformLocation(program[shaderIndex], "Projection");
	GLint mmatLoc = glGetUniformLocation(program[shaderIndex], "ModelView");
	GLint texLoc = glGetUniformLocation(program[shaderIndex], "Texture");
	GLint lLoc = glGetUniformLocation(program[shaderIndex], "LightPosition");
	
	glUseProgram(program[shaderIndex]);
	glBindVertexArray(vaoName[shaderIndex]);

	//input the projection matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	//append the texture into the fragment shader 
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mainTextureID);
	glUniform1i(texLoc, 0);
	
	//std::cout << "light" << light[0] << " " << light[1] << " " << light[2] << std::endl;
	//input the light position into fragment shader
	glUniform4fv(lLoc, 1, light);

	glDrawArrays(GL_TRIANGLES, 0, verticeNumber);

	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(NULL);
	glPopMatrix();

	glutSwapBuffers();
	camera_light_ball_move();
}

//please implement mode toggle(switch mode between phongShading/gouraudShading/flatPhongShading) in case 'b'(lowercase)
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	{	// ESC
		break;
	}
	case 'b': // toggle mode
	{   
		if (shaderIndex == phongIndex) {
			shaderIndex = gouraudIndex;
		}
		else if (shaderIndex == gouraudIndex) {
			shaderIndex = flatIndex;
		}
		else {
			shaderIndex = phongIndex;
		}
		break;
	}
	case 'd':
	{
		right = true;
		break;
	}
	case 'a':
	{
		left = true;
		break;
	}
	case 'w':
	{
		forward = true;
		break;
	}
	case 's':
	{
		backward = true;
		break;
	}
	case 'q':
	{
		up = true;
		break;
	}
	case 'e':
	{
		down = true;
		break;
	}
	case 't':
	{
		lforward = true;
		break;
	}
	case 'g':
	{
		lbackward = true;
		break;
	}
	case 'h':
	{
		lright = true;
		break;
	}
	case 'f':
	{
		lleft = true;
		break;
	}
	case 'r':
	{
		lup = true;
		break;
	}
	case 'y':
	{
		ldown = true;
		break;
	}
	case 'i':
	{
		bforward = true;
		break;
	}
	case 'k':
	{
		bbackward = true;
		break;
	}
	case 'l':
	{
		bright = true;
		break;
	}
	case 'j':
	{
		bleft = true;
		break;
	}
	case 'u':
	{
		bup = true;
		break;
	}
	case 'o':
	{
		bdown = true;
		break;
	}
	case '7':
	{
		bx = true;
		break;
	}
	case '8':
	{
		by = true;
		break;
	}
	case '9':
	{
		bz = true;
		break;
	}
	case '4':
	{
		brx = true;
		break;
	}
	case '5':
	{
		bry = true;
		break;
	}
	case '6':
	{
		brz = true;
		break;
	}

	//special function key
	case 'z'://move light source to front of camera
	{
		light_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180);
		light_pos[1] = eyey + sin(eyet*M_PI / 180);
		light_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180);
		break;
	}
	case 'x'://move ball to front of camera
	{
		ball_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) * 3;
		ball_pos[1] = eyey + sin(eyet*M_PI / 180) * 5;
		ball_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180) * 3;
		break;
	}
	case 'c'://reset all pose
	{
		light_pos[0] = 1.1;
		light_pos[1] = 1.0;
		light_pos[2] = 1.3;
		ball_pos[0] = 0;
		ball_pos[1] = 0;
		ball_pos[2] = 0;
		ball_rot[0] = 0;
		ball_rot[1] = 0;
		ball_rot[2] = 0;
		eyex = 0.0;
		eyey = 0.0;
		eyez = 5.6;
		eyet = 0;
		eyep = 90;
		break;
	}
	default:
	{
		break;
	}
	}
}

//no need to modify the following functions
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.001f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y)
{
	if (mleft)
	{
		eyep -= (x-mousex)*0.1;
		eyet -= (y - mousey)*0.12;
		if (eyet > 89.9)
			eyet = 89.9;
		else if (eyet < -89.9)
			eyet = -89.9;
		if (eyep > 360)
			eyep -= 360;
		else if (eyep < 0)
			eyep += 360;
	}
	mousex = x;
	mousey = y;
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN && !mright && !mmiddle)
		{
			mleft = true;
			mousex = x;
			mousey = y;
		}
		else
			mleft = false;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mmiddle)
		{
			mright = true;
			mousex = x;
			mousey = y;
		}
		else
			mright = false;
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mright)
		{
			mmiddle = true;
			mousex = x;
			mousey = y;
		}
		else
			mmiddle = false;
	}
}

void camera_light_ball_move()
{
	GLfloat dx = 0, dy = 0, dz=0;
	if(left|| right || forward || backward || up || down)
	{ 
		if (left)
			dx = -speed;
		else if (right)
			dx = speed;
		if (forward)
			dy = speed;
		else if (backward)
			dy = -speed;
		eyex += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		eyey += dy*sin(eyet*M_PI / 180);
		eyez += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (up)
			eyey += speed;
		else if (down)
			eyey -= speed;
	}
	if(lleft || lright || lforward || lbackward || lup || ldown)
	{
		dx = 0;
		dy = 0;
		if (lleft)
			dx = -speed;
		else if (lright)
			dx = speed;
		if (lforward)
			dy = speed;
		else if (lbackward)
			dy = -speed;
		light_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		light_pos[1] += dy*sin(eyet*M_PI / 180);
		light_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (lup)
			light_pos[1] += speed;
		else if(ldown)
			light_pos[1] -= speed;
	}
	if (bleft || bright || bforward || bbackward || bup || bdown)
	{
		dx = 0;
		dy = 0;
		if (bleft)
			dx = -speed;
		else if (bright)
			dx = speed;
		if (bforward)
			dy = speed;
		else if (bbackward)
			dy = -speed;
		ball_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		ball_pos[1] += dy*sin(eyet*M_PI / 180);
		ball_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (bup)
			ball_pos[1] += speed;
		else if (bdown)
			ball_pos[1] -= speed;
	}
	if(bx||by||bz || brx || bry || brz)
	{
		dx = 0;
		dy = 0;
		dz = 0;
		if (bx)
			dx = -rotation_speed;
		else if (brx)
			dx = rotation_speed;
		if (by)
			dy = rotation_speed;
		else if (bry)
			dy = -rotation_speed;
		if (bz)
			dz = rotation_speed;
		else if (brz)
			dz = -rotation_speed;
		ball_rot[0] += dx;
		ball_rot[1] += dy;
		ball_rot[2] += dz;
	}
}

void draw_light_bulb()
{
	GLUquadric *quad;
	quad = gluNewQuadric();
	glPushMatrix();
	glColor3f(0.4, 0.5, 0);
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
	gluSphere(quad, light_rad, 40, 20);
	glPopMatrix();
}

void keyboardup(unsigned char key, int x, int y)
{
	switch (key) {
	case 'd':
	{
		right =false;
		break;
	}
	case 'a':
	{
		left = false;
		break;
	}
	case 'w':
	{
		forward = false;
		break;
	}
	case 's':
	{
		backward = false;
		break;
	}
	case 'q':
	{
		up = false;
		break;
	}
	case 'e':
	{
		down = false;
		break;
	}
	case 't':
	{
		lforward = false;
		break;
	}
	case 'g':
	{
		lbackward = false;
		break;
	}
	case 'h':
	{
		lright = false;
		break;
	}
	case 'f':
	{
		lleft = false;
		break;
	}
	case 'r':
	{
		lup = false;
		break;
	}
	case 'y':
	{
		ldown = false;
		break;
	}
	case 'i':
	{
		bforward = false;
		break;
	}
	case 'k':
	{
		bbackward = false;
		break;
	}
	case 'l':
	{
		bright = false;
		break;
	}
	case 'j':
	{
		bleft = false;
		break;
	}
	case 'u':
	{
		bup = false;
		break;
	}
	case 'o':
	{
		bdown = false;
		break;
	}
	case '7':
	{
		bx = false;
		break;
	}
	case '8':
	{
		by = false;
		break;
	}
	case '9':
	{
		bz = false;
		break;
	}
	case '4':
	{
		brx = false;
		break;
	}
	case '5':
	{
		bry = false;
		break;
	}
	case '6':
	{
		brz = false;
		break;
	}

	default:
	{
		break;
	}
	}
}

void idle(void)
{
	glutPostRedisplay();
}

GLuint loadTexture(char* name, GLfloat width, GLfloat height)
{
	return glmLoadTexture(name, false, true, true, true, &width, &height);
}
