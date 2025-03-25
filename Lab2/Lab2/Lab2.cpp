
#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "file.h"

// Section 3: Specify Vertices.
GLfloat vertices[] =
{
	//pos				
	0.f,  0.5f,			1.f, 0.f, 0.f,
	-0.5f, -0.5f,		0.f, 1.f, 0.f,
	0.5f,  -0.5f,		0.f, 0.f, 1.f
};



#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

float x_offset = 0.f;
float y_offset = 0.f;


void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if (type == GL_DEBUG_TYPE_ERROR) {
		fprintf(stderr, "DebugCallback: ERROR - %s\n", message);
	}
}

// Method to register key presses.
void ProcessKeyboard(GLFWwindow* window) {
	// Close window on ESC
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		y_offset += 0.001f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		y_offset -= 0.001f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		x_offset += 0.001f;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		x_offset -= 0.001f;
}

// Define a callback for resizing a window.
void SizeCallback(GLFWwindow* window, int w, int h) {
	// w, h new size of the window.
	glViewport(0, 0, w, h);
}

int main()
{	
	// Section 1: Make a Window.
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(640, 480, "A Triangle", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, SizeCallback);

	// Section 2: Initialise OpenGL.
	gl3wInit();

	// Enable debugging.
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	// Section 4: Compile Shaders.
	GLuint program = CompileShader("triangle.vert", "triangle.frag");

	// Section 5: Create buffers, Send to GPU.
	// Creates n buffers in VRAM, store in buffers array.
	glCreateBuffers(NUM_BUFFERS, Buffers);
	// Allocates "size" bytes in the buffer objects data store, copies data from RAM to VRAM. 
	glNamedBufferStorage(Buffers[0], sizeof(vertices), vertices, 0);
	// Creates n VAOs and stores in arrays array.
	glGenVertexArrays(NUM_VAOS, VAOs);
	// Binds the VAO array so that changes can be made to that VAO.
	glBindVertexArray(VAOs[0]);
	// Binds the buffer to target.
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);

	// Each attribute of the vertex attribute array needs to be described in the VAO.

	// Specifies that the index attribute has size number of components of the specified type, where 
	// each vertex is made from stride bytes, and offset is the index of the first element of this attribute
	// after the start of particular vertex.
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (5 * sizeof(float)), (void*)0);
	// Enables the attribute at given index.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (5 * sizeof(float)), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Section 6: Render Loop.
	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);

		glUseProgram(program);

		glUniform1f(glGetUniformLocation(program, "x_offset"), x_offset);
		glUniform1f(glGetUniformLocation(program, "y_offset"), y_offset);

		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
		ProcessKeyboard(window);
	}

	// Section 7: Deinitialisation.
	glfwDestroyWindow(window);
	glfwTerminate();
}
