
#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "error.h"
#include "file.h"
#include "shader.h"


#define NUM_BUFFERS 2
#define NUM_VAOS 2
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

float vertices1[] =
{
	//pos							col
	0.f, 0.25f, 0.f,  				0.f, 1.f, 0.f,	//t
	-0.25f, -0.25f, 0.f,  			0.f, 1.f, 0.f,	//bl
	0.25f,  -0.25f, 0.f,  			0.f, 1.f, 0.f	//br
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

	// For red triangle
	glNamedBufferStorage(Buffers[0], sizeof(vertices0), vertices0, 0);
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// For green triangle
	glNamedBufferStorage(Buffers[1], sizeof(vertices1), vertices1, 0);
	glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);



	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);

		glUseProgram(program);

		glm::mat4 M0 = glm::mat4(1.f);
		M0 = glm::translate(M0, glm::vec3(0.5, 0, 0));
		M0 = glm::rotate(M0, glm::radians(90.f), glm::vec3(0, 0, 1));
		glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, glm::value_ptr(M0));
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);


		// Init identity matrix
		glm::mat4 M1 = glm::mat4(1.f);
		// Translate triangle
		M1 = glm::rotate(M1, glm::radians(90.f), glm::vec3(0, 0, 1));
		M1 = glm::translate(M1, glm::vec3(0.5, 0, 0));
		// Get uniform location and copy matrix to uniform in shader
		glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, glm::value_ptr(M1));
		// Bind appropriate VAO
		glBindVertexArray(VAOs[1]);
		// Draw triangle
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwDestroyWindow(window);
	glfwTerminate();
}