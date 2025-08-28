#define _USE_MATH_DEFINES 

#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "error.h"
#include "file.h"
#include "shader.h"
#include "math.h"


#define NUM_BUFFERS 1
#define NUM_VAOS 1
#define DEG2RAD(n) n*(M_PI/180)
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

}

void ResizeCallback(GLFWwindow*, int w, int h)
{
	glViewport(0, 0, w, h);
}


#define DEG2RAD(n)	n*(M_PI/180)

//DEFINE YOUR FUNCTION FOR CREATING A CIRCLE HERE
// Returns an array of vertices which will model a circle made of triangles.
float* CreateCircle(int num_segments, float radius) {
	float* vert_array = (float*)malloc((9 * num_segments) * sizeof(float));
	float offset = 360.0f / num_segments;
	float angle = 0.0f;

	int increment = 0;
	for (int i = 0; i < num_segments; i++) {
		// v0
		vert_array[increment++] = 0.f;
		vert_array[increment++] = 0.f;
		vert_array[increment++] = 0.f;

		// v1
		vert_array[increment++] = radius * cos(DEG2RAD(angle));
		vert_array[increment++] = radius * sin(DEG2RAD(angle));
		vert_array[increment++] = 0.f;

		angle += offset;

		// v2
		vert_array[increment++] = radius * cos(DEG2RAD(angle));
		vert_array[increment++] = radius * sin(DEG2RAD(angle));
		vert_array[increment++] = 0.f;

	}
	return vert_array;
}

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(640, 480, "2D modelling", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetWindowSizeCallback(window, ResizeCallback);

	gl3wInit();


	GLuint program = CompileShader("triangle.vert", "triangle.frag");

	//CREATE CIRCLE HERE
	int num_segs = 32;
	float* verts = CreateCircle(num_segs, 0.5f);
	
	glCreateBuffers(NUM_BUFFERS, Buffers);
	//COPY VERTICES HERE
	glNamedBufferStorage(Buffers[0], (9 * num_segs) * sizeof(float), verts, NULL);

	glGenVertexArrays(NUM_VAOS, VAOs);
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	//SETUP ATTRIBUTES HERE
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 * sizeof(float)), (void*)0);
	glEnableVertexAttribArray(0);


	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);

		glUseProgram(program);
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, num_segs*3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//FREE THE ALLOCATED MEMORY HERE
	free(verts);

	glfwDestroyWindow(window);
	glfwTerminate();
}