#include <iostream>
#include "glew.h"
#include "glut.h"
#include <gl\GL.h>
#include "math_3d.h"
#include "shader.h"

int const winWidth = 480;
int const winHeight = 480;

GLuint VBO_Handle;					// this buffer will store the handle of the actual VBO; it REPRESENTS the VBO in this program, but it's not the VBO

static GLuint gWorldLocation;

static void initScene(void)
{
	Vector3f v[] = {									// to draw a triangle, create 3 vertices on normalized positions
		Vector3f(-0.5f, -0.5f, 0.0f),
		Vector3f(0.5f, -0.5f, 0.0f),
		Vector3f(0.0f, 0.5f, 0.0f)
	};

	glGenBuffers(1, &VBO_Handle);								// let's generate a buffer to bind at our VBO inside the GPU
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Handle);					// and let's bind it...the option GL_ARRAY_BUFFER says that the array will contain only vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), &v, GL_STATIC_DRAW);	// let's fill the buffer with data
}

static void RenderScene(void)
{
	static float angle = 0.0f;
	Matrix4f world;

	glClear(GL_COLOR_BUFFER_BIT);						// use the color specified by glClearColor

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Handle);					// let's bind it again to prepare for draw
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// index = 0, num components = 3 (X, Y, Z), data type of the components = float
	
	glDrawArrays(GL_TRIANGLES,0,3);						//let's draw a triangle
	
	glDisableVertexAttribArray(0);

	glutSwapBuffers();									// swap the 2 buffers so the current buffer will go in background, 
														// whereas the old background buffer will be shown

	/* For each frame we generate a rotation matrix that moves the object around an axis according to the rotation angle a and the plane we are looking at 
	   The roation matrices are made as following:
		
		XY plane (rotate around Z axis)
		---                             ---     ---   ---
		|  cos(a)	-sin(a)		0	   0  |     | Xpos  |
		|  sin(a)	cos(a)		0	   0  |     | Ypos  |
		|    0		  0			1	   0  |  *  | Zpos  |
		|    0		  0			0	   1  |     |   1   |
		---                             ---     ---   ---

		XZ plane (rotate around Y axis)
		---                             ---     ---   ---
		|  cos(a)	  0		 -sin(a)   0  |     | Xpos  |
		|    0		  1			0	   0  |     | Ypos  |
		|  sin(a)	  0		  cos(a)   0  |  *  | Zpos  |
		|    0		  0			0	   1  |     |   1   |
		---                             ---     ---   ---

		YZ plane (rotate around X axis)
		---                             ---     ---   ---
		|    1		  0			0	   0  |     | Xpos  |
		|    0		cos(a)	  -sin(a)  0  |     | Ypos  |
		|    0		sin(a)	  cos(a)   0  |  *  | Zpos  |
		|    0		  0			0	   1  |     |   1   |
		---                             ---     ---   ---
	*/
	angle += 0.01f;
	//Let's rotate around Z-axis
	world.m[0][0] = cosf(angle);
	world.m[0][1] = -sinf(angle);
	world.m[0][2] = 0.0f;
	world.m[0][3] = 0.0f;
	world.m[1][0] = sinf(angle);
	world.m[1][1] = cosf(angle);
	world.m[1][2] = 0.0f;
	world.m[1][3] = 0.0f;
	world.m[2][0] = 0.0f;
	world.m[2][1] = 0.0f;
	world.m[2][2] = 1.0f;
	world.m[2][3] = 0.0f;
	world.m[3][0] = 0.0f;
	world.m[3][1] = 0.0f;
	world.m[3][2] = 0.0f;
	world.m[3][3] = 1.0f;
	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &world.m[0][0]);					// update vertex shader's gScale variable with a new value

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