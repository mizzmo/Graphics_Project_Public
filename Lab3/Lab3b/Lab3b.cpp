
#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>



#include "error.h"
#include "file.h"
#include "shader.h"


#define NUM_BUFFERS 1
#define NUM_VAOS 1
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


float vertices0[] =
{
	//pos							col
	0.f, 0.25f, 0.f,  				1.f, 0.f, 0.f,	//t
	-0.25f, -0.25f, 0.f,  			1.f, 0.f, 0.f,	//bl
	0.25f,  -0.25f, 0.f,  			1.f, 0.f, 0.f	//br
};


int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(640, 480, "2D Transformations", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetWindowSizeCallback(window, ResizeCallback);

	gl3wInit();


	GLuint program = CompileShader("triangle.vert", "triangle.frag");


	glCreateBuffers(NUM_BUFFERS, Buffers);
	glGenVertexArrays(NUM_VAOS, VAOs);

	glNamedBufferStorage(Buffers[0], sizeof(vertices0), vertices0, 0);
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);



	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);

		glUseProgram(program);

		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwDestroyWindow(window);
	glfwTerminate();
}