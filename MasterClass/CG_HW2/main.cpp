/*

	CG Homework2 - Shadow Mapping & Dissolve Effects

	Objective - learning Shadow Implmentation and Dissolve Effects

	Overview:

		1. Render the model with Shadow using shadow mapping

		2. Implement dissolve effect

	!!!IMPORTANT!!! 

	1. Make sure to change the window name to your student ID!
	2. You are allow to use glmDraw this time.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /*for function: offsetof */
#include <math.h>
#include <string.h>
#include "../GL/glew.h"
#include "../GL/glut.h"
#include "../shader_lib/shader.h"
#include "glm/glm.h"
#include <iostream>

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

void initDepthMap(void);
Vertex* saveVerticesDataFromModel(GLMmodel* model);
void createBallShader(void);
void createPlaneShader(void);
void createRabbitShader(void);
void createDepthShader(void);

// Create depth map in display()
void createBallDepthMap(void);
void createPlaneDepthMap(void);
void createRabbitDepthMap(void);

// Render scene
void drawBall(void);
void drawPlane(void);
void drawRabbit(void);

namespace
{
	char *obj_file_dir = "../Resources/Ball.obj";
	char *obj_file_dir2 = "../Resources/bunny.obj";
	char *main_tex_dir = "../Resources/Stone.ppm";
	char *floor_tex_dir = "../Resources/WoodFine.ppm";
	char *plane_file_dir = "../Resources/Plane.obj";
	char *noise_tex_dir = "../Resources/noise.ppm";
	
	GLfloat light_rad = 0.05; //radius of the light bulb
	float eyet = -5.59; //theta in degree
	float eyep = 83.2; //phi in degree
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
const float speed = 0.03; // camera / light / ball moving speed
const float rotation_speed = 0.05; // ball rotating speed

//you may need to use some of the following variables in your program 

// No need for model texture, 'cause glmModel() has already loaded it for you.
// To use the texture, check glmModel documentation.
GLuint mainTextureID; // TA has already loaded this texture for you
GLuint floorTextureID;
GLuint noiseTextureID;

GLMmodel *ballModel; // TA has already loaded the model for you(!but you still need to convert it to VBO(s)!)
GLMmodel *planeModel;
GLMmodel *rabbitModel;

float eyex = -3.291;
float eyey = 1.57;
float eyez = 11.89;

GLfloat light_pos[] = { 1.1, 3.5, 1.3 };
GLfloat ball_pos[] = { 0.0, 0.0, 0.0 };
GLfloat ball_rot[] = { 0.0, 0.0, 0.0 };
GLfloat plane_pos[] = { 0.0, -5.0, 0.0 };
GLfloat plane_rot[] = { 0.0, 0.0, 0.0 };
GLfloat rabbit_pos[] = { -2.295, -5.0, -2.0 };
GLfloat rabbit_rot[] = { 0.0, 0.0, 0.0 };
GLuint rabbitIndex = 0;
GLuint ballIndex = 1;
GLuint planeIndex = 2;
GLuint depthIndex = 3;
GLuint vaoName[4];
GLuint vboName[4];
GLuint program[4];

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	
	// remember to replace "YourStudentID" with your own student ID
	glutCreateWindow("CG_HW2_0856601");
	glutReshapeWindow(512, 512);

	glewInit();

	initDepthMap();
	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();

	glmDelete(ballModel);
	return 0;
}

void init(void)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glEnable(GL_CULL_FACE);

	mainTextureID = loadTexture(main_tex_dir, 1024, 1024);
	floorTextureID = loadTexture(floor_tex_dir, 512, 512);
	noiseTextureID = loadTexture(noise_tex_dir, 320, 320);

	ballModel = glmReadOBJ(obj_file_dir);
	glmUnitize(ballModel);
	glmFacetNormals(ballModel);
	glmVertexNormals(ballModel, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(ballModel);

	planeModel = glmReadOBJ(plane_file_dir);
	glmFacetNormals(planeModel);
	glmVertexNormals(planeModel, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(planeModel);	

	rabbitModel = glmReadOBJ(obj_file_dir2);
	glmFacetNormals(rabbitModel);
	glmVertexNormals(rabbitModel, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(rabbitModel);
    
	// you may need to do something here(create shaders/program(s) and create vbo(s)/vao from GLMmodel model)

	// APIs for creating shaders and creating shader programs have been done by TAs
	// following is an example for creating a shader program using given vertex shader and fragment shader
	//Generate a new buffer object
	glGenBuffers(3, vboName);
	glGenVertexArrays(3, vaoName);
	createRabbitShader();
	createBallShader();
	createPlaneShader();
	createDepthShader();
}

void createRabbitShader(void) {
	GLuint vert = createShader("Shaders/shadow.vert", "vertex");
	GLuint frag = createShader("Shaders/shadow.frag", "fragment");
	program[rabbitIndex] = createProgram(vert, frag);
	
	glBindVertexArray(vaoName[rabbitIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, vboName[rabbitIndex]);

	Vertex *vertices = saveVerticesDataFromModel(rabbitModel);
	GLuint rabbitVNum = rabbitModel->numtriangles * 3;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * rabbitVNum, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void createDepthShader(void) {
	GLuint vert = createShader("Shaders/depth.vert", "vertex");
	GLuint frag = createShader("Shaders/depth.frag", "fragment");
	program[depthIndex] = createProgram(vert, frag);
}

void createBallShader(void) {
	GLuint vert = createShader("Shaders/shadow.vert", "vertex");
	GLuint frag = createShader("Shaders/shadow.frag", "fragment");
	program[ballIndex] = createProgram(vert, frag);

	glBindVertexArray(vaoName[ballIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, vboName[ballIndex]);

	Vertex *vertices = saveVerticesDataFromModel(ballModel);
	GLuint ballVNum = ballModel->numtriangles * 3;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * ballVNum, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void createPlaneShader(void) {
	GLuint vert = createShader("Shaders/shadow.vert", "vertex");
	GLuint frag = createShader("Shaders/shadow.frag", "fragment");
	program[planeIndex] = createProgram(vert, frag);

	glBindVertexArray(vaoName[planeIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, vboName[planeIndex]);

	Vertex *vertices = saveVerticesDataFromModel(planeModel);
	GLuint planeVNum = planeModel->numtriangles * 3;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * planeVNum, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Vertex* saveVerticesDataFromModel(GLMmodel* model) {
	GLuint triangleNumber = model->numtriangles;
	GLuint verticeNumber = triangleNumber * 3;
	Vertex *vertices = new Vertex[verticeNumber];
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

	return vertices;
}

unsigned int depthMapFBO;
unsigned int depthMap;
void initDepthMap(void)
{   
	// Create 2D texture as the frame buffers depth buffer
	const unsigned int SHADOW_WIDTH = 512, SHADOW_HEIGHT = 512;
	// Create frame buffer for rendering depth map
	glGenFramebuffers(1, &depthMapFBO);
	
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}

GLfloat ballDepthMV[16];
GLfloat planeDepthMV[16];
GLfloat rabbitDepthMV[16];
GLfloat lightProjectionMatrix[16];
void display(void)
{    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//you may need to do something here(declare some local variables you need and maybe load Model matrix here...)
	// Get the projection matrix from the light
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	    glLoadIdentity();
	    glOrtho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	    glGetFloatv(GL_PROJECTION_MATRIX, lightProjectionMatrix);
	glPopMatrix();

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	createPlaneDepthMap();
	glClear(GL_DEPTH_BUFFER_BIT);
	createBallDepthMap();
	createRabbitDepthMap();
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);

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
		0.0
	);

	glPushMatrix();
		glColor3f(1, 1, 1);
		draw_light_bulb();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);
		glRotatef(ball_rot[0], 1, 0, 0);
		glRotatef(ball_rot[1], 0, 1, 0);
		glRotatef(ball_rot[2], 0, 0, 1);
		glColor3f(1, 1, 1);
		// you may need to do something here(pass uniform variable(s) to shader and render the model)
		drawBall();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(plane_pos[0], plane_pos[1], plane_pos[2]);
		glRotatef(plane_rot[0], 1, 0, 0);
		glRotatef(plane_rot[1], 0, 1, 0);
		glRotatef(plane_rot[2], 0, 0, 1);
		glColor3f(1, 1, 1);
		// you may need to do something here(pass uniform variable(s) to shader and render the model)
		drawPlane();
	glPopMatrix();

    glPushMatrix();
	   glTranslatef(rabbit_pos[0], rabbit_pos[1], rabbit_pos[2]);
	   glRotatef(rabbit_rot[0], 1, 0, 0);
	   glRotatef(rabbit_rot[1], 0, 1, 0);
	   glRotatef(rabbit_rot[2], 0, 0, 1);
	   // you may need to do something here(pass uniform variable(s) to shader and render the model)
	   drawRabbit();
	glPopMatrix();

	glutSwapBuffers();
	camera_light_ball_move();
}

void createBallDepthMap() {
	// Get the View matrix from the light 
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	    glLoadIdentity();
	    gluLookAt(
	        light_pos[0],
		    light_pos[1],
		    light_pos[2],
		    // Watch at original
		    0.0,
		    0.0,
		    0.0,
		    // Up vector
		    0.0,
		    1.0,
		    0.0
		);

	    //  Get the model matrix of ball
	    glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);
	    glRotatef(ball_rot[0], 1, 0, 0);
	    glRotatef(ball_rot[1], 0, 1, 0);
	    glRotatef(ball_rot[2], 0, 0, 1);

	    glGetFloatv(GL_MODELVIEW_MATRIX, ballDepthMV);
	    GLint mmatLoc = glGetUniformLocation(program[depthIndex], "ModelView");
	    GLint pmatLoc = glGetUniformLocation(program[depthIndex], "Projection");

	    glUseProgram(program[depthIndex]);
	    // Use ball's VAO
	    glBindVertexArray(vaoName[ballIndex]);

	    // input the projection matrix into vertex shader
	    glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, lightProjectionMatrix);
	    // input the modelview matrix into vertex shader
	    glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, ballDepthMV);
	    
	    GLuint ballVNum = ballModel->numtriangles * 3;
	    glDrawArrays(GL_TRIANGLES, 0, ballVNum);
	    glUseProgram(NULL);
	glPopMatrix();
}

void createPlaneDepthMap() {
	// Get the View matrix from the light 
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	    glLoadIdentity();
	    gluLookAt(
		    light_pos[0],
		    light_pos[1],
		    light_pos[2],
		    // Watch at original
		    0.0,
		    0.0,
		    0.0,
		    // Up vector
		    0.0,
		    1.0,
		    0.0
		);

	    // Get the model matrix of plane
	    glTranslatef(plane_pos[0], plane_pos[1], plane_pos[2]);
	    glRotatef(plane_rot[0], 1, 0, 0);
	    glRotatef(plane_rot[1], 0, 1, 0);
	    glRotatef(plane_rot[2], 0, 0, 1);
	    
	    glGetFloatv(GL_MODELVIEW_MATRIX, planeDepthMV);
	    GLint mmatLoc = glGetUniformLocation(program[depthIndex], "ModelView");
	    GLint pmatLoc = glGetUniformLocation(program[depthIndex], "Projection");
	    
	    glUseProgram(program[depthIndex]);
	    // Use ball's VAO
	    glBindVertexArray(vaoName[planeIndex]);
	    
	    //input the projection matrix into vertex shader
	    glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, lightProjectionMatrix);
	    //input the modelview matrix into vertex shader
	    glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, planeDepthMV);
	    
	    GLuint planeVNum = planeModel->numtriangles * 3;
	    glDrawArrays(GL_TRIANGLES, 0, planeVNum);
	    glUseProgram(NULL);
	glPopMatrix();
}

void createRabbitDepthMap(void) {
	// Get the View matrix from the light 
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	    glLoadIdentity();
	    gluLookAt(
	    	light_pos[0],
	    	light_pos[1],
	    	light_pos[2],
	    	// Watch at original
	    	0.0,
	    	0.0,
	    	0.0,
	    	// Up vector
	    	0.0,
	    	1.0,
	    	0.0
	    );
	    
	    // Get the model matrix of rabbit
	    glTranslatef(rabbit_pos[0], rabbit_pos[1], rabbit_pos[2]);
	    glRotatef(rabbit_rot[0], 1, 0, 0);
	    glRotatef(rabbit_rot[1], 0, 1, 0);
	    glRotatef(rabbit_rot[2], 0, 0, 1);
	    
		// Get the model view of rabbit in the light space
	    glGetFloatv(GL_MODELVIEW_MATRIX, rabbitDepthMV);

	    GLint mmatLoc = glGetUniformLocation(program[depthIndex], "ModelView");
	    GLint pmatLoc = glGetUniformLocation(program[depthIndex], "Projection");
	    
	    glUseProgram(program[depthIndex]);
	    // Use rabitt's VAO
	    glBindVertexArray(vaoName[rabbitIndex]);
	    
	    //input the projection matrix into vertex shader
	    glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, lightProjectionMatrix);
	    //input the modelview matrix into vertex shader
	    glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, rabbitDepthMV);
	    
	    GLuint rabbitVNum = rabbitModel->numtriangles * 3;
	    glDrawArrays(GL_TRIANGLES, 0, rabbitVNum);
	    glUseProgram(NULL);
	glPopMatrix();
}

void drawBall(void) {
	
	GLfloat pmtx[16];
	GLfloat mmtx[16];

    glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	GLint pmatLoc = glGetUniformLocation(program[ballIndex], "Projection");
	GLint mmatLoc = glGetUniformLocation(program[ballIndex], "ModelView");
	GLint depthMVLoc = glGetUniformLocation(program[ballIndex], "DepthMV");
	GLint depthPLoc = glGetUniformLocation(program[ballIndex], "DepthP");
	GLint lightPosLoc = glGetUniformLocation(program[ballIndex], "LightPos");
	GLint texLoc = glGetUniformLocation(program[ballIndex], "Texture");
	GLint depthTexLoc = glGetUniformLocation(program[ballIndex], "DepthTexture");

	glUseProgram(program[ballIndex]);
	glBindVertexArray(vaoName[ballIndex]);

	//input the projection matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);
	//Input the modelview matrix to light space into vertex shader
	glUniformMatrix4fv(depthMVLoc, 1, GL_FALSE, ballDepthMV);
	//Input the projection matrix to light space into vertex shader
	glUniformMatrix4fv(depthPLoc, 1, GL_FALSE, lightProjectionMatrix);
	//Input light position for solving shadow acne
	glUniform3fv(lightPosLoc, 1, light_pos);

	//append the texture into the fragment shader 
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mainTextureID);
	glUniform1i(texLoc, 0);
	
	//append the depth texture into the fragment shader 
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniform1i(depthTexLoc, 1);

	GLuint ballVNum = ballModel->numtriangles * 3;
	glDrawArrays(GL_TRIANGLES, 0, ballVNum);

	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(NULL);
}

void drawPlane(void) {
	GLfloat pmtx[16];
	GLfloat mmtx[16];

	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	GLint pmatLoc = glGetUniformLocation(program[planeIndex], "Projection");
	GLint mmatLoc = glGetUniformLocation(program[planeIndex], "ModelView");
	GLint depthMVLoc = glGetUniformLocation(program[planeIndex], "DepthMV");
	GLint depthPLoc = glGetUniformLocation(program[planeIndex], "DepthP");
	GLint lightPosLoc = glGetUniformLocation(program[planeIndex], "LightPos");
	GLint texLoc = glGetUniformLocation(program[planeIndex], "Texture");
	GLint depthTexLoc = glGetUniformLocation(program[planeIndex], "DepthTexture");

	glUseProgram(program[planeIndex]);
	glBindVertexArray(vaoName[planeIndex]);

	//input the projection matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);
	//Input the modelview matrix to light space into vertex shader
	glUniformMatrix4fv(depthMVLoc, 1, GL_FALSE, planeDepthMV);
	//Input the projection matrix to light space into vertex shader
	glUniformMatrix4fv(depthPLoc, 1, GL_FALSE, lightProjectionMatrix);
	//Input light position for solving shadow acne
	glUniform3fv(lightPosLoc, 1, light_pos);

	// append the texture into the fragment shader
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTextureID);
	glUniform1i(texLoc, 0);

	//append the depth texture into the fragment shader 
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniform1i(depthTexLoc, 1);

	GLuint planeVNum = planeModel->numtriangles * 3;
	glDrawArrays(GL_TRIANGLES, 0, planeVNum);
	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(NULL);
}

void drawRabbit(void) {
	GLfloat pmtx[16];
	GLfloat mmtx[16];

	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	GLint pmatLoc = glGetUniformLocation(program[rabbitIndex], "Projection");
	GLint mmatLoc = glGetUniformLocation(program[rabbitIndex], "ModelView");
	GLint depthMVLoc = glGetUniformLocation(program[rabbitIndex], "DepthMV");
	GLint depthPLoc = glGetUniformLocation(program[rabbitIndex], "DepthP");
	GLint lightPosLoc = glGetUniformLocation(program[rabbitIndex], "LightPos");
	GLint texLoc = glGetUniformLocation(program[rabbitIndex], "Texture");
	GLint depthTexLoc = glGetUniformLocation(program[rabbitIndex], "DepthTexture");

	glUseProgram(program[rabbitIndex]);
	glBindVertexArray(vaoName[rabbitIndex]);

	//input the projection matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);
	//Input the modelview matrix to light space into vertex shader
	glUniformMatrix4fv(depthMVLoc, 1, GL_FALSE, rabbitDepthMV);
	//Input the projection matrix to light space into vertex shader
	glUniformMatrix4fv(depthPLoc, 1, GL_FALSE, lightProjectionMatrix);
	//Input light position for solving shadow acne
	glUniform3fv(lightPosLoc, 1, light_pos);

	// append the texture into the fragment shader
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mainTextureID);
	glUniform1i(texLoc, 0);

	//append the depth texture into the fragment shader 
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniform1i(depthTexLoc, 1);

	GLuint rabbitVNum = rabbitModel->numtriangles * 3;
	glDrawArrays(GL_TRIANGLES, 0, rabbitVNum);
	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(NULL);
}

// please implement mode increase/decrease dissolve threshold in case '-' and case '=' (lowercase)
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	{	// ESC
		break;
	}
	case '-': // increase dissolve threshold
	{
		// you may need to do somting here
		break;
	}
	case '=': // decrease dissolve threshold
	{
		// you may need to do somting here
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
		light_pos[1] = 3.5;
		light_pos[2] = 1.3;
		ball_pos[0] = 0;
		ball_pos[1] = 0;
		ball_pos[2] = 0;
		ball_rot[0] = 0;
		ball_rot[1] = 0;
		ball_rot[2] = 0;
		eyex = -3.291;
		eyey = 1.57;
		eyez = 11.89;
		eyet = -5.59; //theta in degree
		eyep = 83.2; //phi in degree
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
	glColor3f(1, 1, 0);
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
	rabbit_rot[1] += 1;
	glutPostRedisplay();
}

GLuint loadTexture(char* name, GLfloat width, GLfloat height)
{
	return glmLoadTexture(name, false, true, true, true, &width, &height);
}
