#include <iostream>
#include "glew.h"
#include "glut.h"
#include <gl\GL.h>
#include "math_3d.h"
#include "shader.h"
#include "Pipeline.h"

enum {
	POSITION,
	COLOR
};

int const winWidth = 480;
int const winHeight = 480;

GLuint VBO_Handle[2];					// this buffer will store the handle of the actual VBO; it REPRESENTS the VBO in this program, but it's not the VBO
GLuint IBO_Handle;
static GLuint gWorldLocation;

static void initScene(void)
{
	Vector3f v[] = {					
		Vector3f(-0.5f, -0.5f,  0.5f),		    // left front
		Vector3f(-0.5f, -0.5f, -0.5f),		    // left rear
		Vector3f( 0.5f, -0.5f,  0.5f),			// right front
		Vector3f( 0.5f, -0.5f, -0.5f),		    // right rear
		Vector3f( 0.0f,  0.5f,  0.0f)			// peak
	};

	Vector3f c[] = {
		Vector3f(0.0f, 0.0f, 1.0f),
		Vector3f(0.0f, 0.0f, 1.0f),
		Vector3f(0.0f, 1.0f, 0.0f),
		Vector3f(0.0f, 1.0f, 0.0f),
		Vector3f(1.0f, 0.0f, 0.0f),
	};

	GLuint indices[] = {
		0, 1, 3,
		0, 3, 2,
		0, 1, 4,
		1, 3, 4,
		3, 2, 4,
		2, 0, 4
	};

	glGenBuffers(2, VBO_Handle);								// let's generate a buffer to bind at our VBO inside the GPU
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Handle[POSITION]);					
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), &v, GL_STATIC_DRAW);	

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Handle[COLOR]);					
	glBufferData(GL_ARRAY_BUFFER, sizeof(c), &c, GL_STATIC_DRAW);

	glGenBuffers(1, &IBO_Handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Handle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	glEnable(GL_DEPTH_TEST);
}

static void RenderScene(void)
{
	static float scale = 0.0f;
	Pipeline p;
	

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


	glEnableVertexAttribArray(POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Handle[POSITION]);					// let's bind it again to prepare for draw
	glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);	// index = 0, num components = 3 (X, Y, Z), data type of the components = float

	glEnableVertexAttribArray(COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Handle[COLOR]);					// let's bind it again to prepare for draw
	glVertexAttribPointer(COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0);	// index = 0, num components = 3 (X, Y, Z), data type of the components = float
		
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Handle);

	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(POSITION);
	glDisableVertexAttribArray(COLOR);

	glutSwapBuffers();									// swap the 2 buffers so the current buffer will go in background, 
		
	scale += 0.01f;
	p.setTranslation(0.0f, 0.0f, 0.0f);
	p.setScaling(1.0f, 1.0f, 1.0f);
	p.setRotation(scale, 0.0, scale);
	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (const GLfloat*)p.getMatrix());	// update vertex shader's gScale variable with a new value

	glutPostRedisplay();								// call the openGL render function (otherwise the window won't be rendered again but only redrawn);
}

int main(int argc, char **argv) 
{
	GLuint program;

	glutInit (&argc, argv);								// initialize GLUT

	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA);		// initialize the display to support double buffering and RGBA colors
	glutInitWindowSize (winWidth, winHeight);			// set initial size of the window
	glutInitWindowPosition (0, 0);						// set initial position of the window
	glutCreateWindow("OpenGL tutorial");				// create a window with OpenGL tutorial as title

	glutDisplayFunc(RenderScene);						// define the render function
	glutIdleFunc(RenderScene);

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return -1;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// set the clear Display state to the black color; the windows will use this setting during the whole execution

	initScene();

	program = compileShaders();

	gWorldLocation = glGetUniformLocation(program,"gWorld"); // get the index of the vertex shader where to find the gScale uniform variable
															//knowing the index we can change this value from our application and see the shape moving

	glutMainLoop();

	return 0;
}