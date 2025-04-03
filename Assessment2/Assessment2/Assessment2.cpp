#include<iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "error.h"
#include "shader.h"


#define NUM_VBO 1

#define NUM_VAO 1


// Create a Vertex Array Object (VAO)
// Stores pointers to VBOs and tells OpenGL how to interpret the data.
// Need to make before VBO, can be used to switch between them
GLuint VAOs[NUM_VAO];
// Store vertex data in vertex buffer object.
GLuint VBOs[NUM_VBO];


// Create an array of floats representing coordinates of a triangle.
GLfloat vertices[] = {
	// Left Corner, Right Corner, Top Left Corner
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f,
};

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Handle key presses
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void ResizeCallback(GLFWwindow*, int w, int h)
{
	// Handle window resizes.
	// Area we want to render in in the window.
	// Coordinates and window size.
	glViewport(0, 0, w, h);
}

int main() {
	// Initialize GLFW
	glfwInit();

	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(800, 600, "Assessment2", NULL, NULL);

	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Tell GL to use the window.
	// Add to context.
	glfwMakeContextCurrent(window);
	// Set what happens when keys are pressed
	glfwSetKeyCallback(window, KeyCallback);
	// Set callback for window resize
	glfwSetWindowSizeCallback(window, ResizeCallback);

	// Init gl3w after making context.
	if (gl3wInit()) {
		std::cout << "Failed to initialize GL3W" << std::endl;
		return -1;
	}

	// Create a vertext shader and Fragment Shader using the Shader class.
	GLuint program = CompileShader("vertex_shader.vert", "fragment_shader.frag");
	
	// Generate number of VAOs
	glGenVertexArrays(NUM_VAO, VAOs);
	// Bind VAO
	glBindVertexArray(VAOs[0]);
	// Generate number of VBOs
	glCreateBuffers(NUM_VBO, VBOs);
	// Binding makes a certain object the current object.
	// Modifying an object modifies the current bound object. 
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	// Store vertices in VBO
	// Format :: (Type of buffer, size of buffer, pointer to data, usage hint)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Configure VAO so gl knows how to read it(
	// Pos of attribute, how many values per vertex, kind of values, coordinates as ints?, stride (data between each vertex), offset (pointer to where vertices start in array)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Enable the vertex attribute array
	glEnableVertexAttribArray(0);
	
	// Unbind both by binding to 0
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	while (!glfwWindowShouldClose(window)) {
		// Clear the colour buffer and give another colour.
		glClearColor(0.07f, 0.31f, 0.17f, 1.0f);
		// Execute colour buffer command.
		glClear(GL_COLOR_BUFFER_BIT);
		// Actiate shader program
		glUseProgram(program);
		// Bind the VAO to tell gl we want to use this one
		glBindVertexArray(VAOs[0]);

		// Draw the triangle
		// Type of primitive, starting value, amount of vertices.
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Swap buffers to update image each frame.
		glfwSwapBuffers(window);

		// Process polled events (resize etc).
		glfwPollEvents();
	}

	// Remove objects
	glDeleteVertexArrays(NUM_VAO, VAOs);
	glDeleteBuffers(NUM_VBO, VBOs);
	// Delete the shader program
	glDeleteProgram(program);

	// Remove the window
	glfwDestroyWindow(window);
	// Terminate GLFW
	glfwTerminate();
	return 0;
}