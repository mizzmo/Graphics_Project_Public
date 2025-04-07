#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "error.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"

#define NUM_VBO 1
#define NUM_VAO 1

SCamera Camera;



// Create a Vertex Array Object (VAO)
// Stores pointers to VBOs and tells OpenGL how to interpret the data.
// Need to make before VBO, can be used to switch between them
GLuint VAOs[NUM_VAO];
// Store vertex data in vertex buffer object.
GLuint VBOs[NUM_VBO];

// Create an array of floats representing coordinates of a triangle.
GLfloat vertices[] = {
	// Coordinates, Colours, Texture
	-1.0f, 0.0f, -1.0f,  	1.f, 0.0f, 0.0f,		0.0f, 0.0f,				//v1
	-1.0f, 0.0f, 1.0f,      1.f, 0.0f, 0.0f,		1.0f, 0.0f,				//v2
	0.0f, 1.0f, 0.0f,		1.f, 0.0f, 0.0f,		0.0f, 1.0f,				//v3

	//TB
	//pos					//col			
	1.0f, 0.0f, -1.0f,		0.f, 1.f, 0.0f,			0.0f, 0.0f,				//v1
	-1.0f, 0.0f, -1.0f,		0.f, 1.f, 0.0f, 		1.0f, 0.0f,				//v2
	0.0f, 1.0f, 0.0f,		0.0f, 1.f, 0.0f, 		0.0f, 1.0f,				//v3

	//TR
	//pos					//col			
	1.0f, 0.0f, 1.0f,  		0.0f, 0.0f, 1.f,		0.0f, 0.0f,				//v1
	1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 1.f,		1.0f, 0.0f,				//v2
	0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.f,		0.0f, 1.0f,				//v3

	//TF
	//pos					//col			
	-1.0f, 0.0f, 1.0f,  	1.f, 1.f, 0.0f,			0.0f, 0.0f,				//v1
	1.0f, 0.0f, 1.0f,		1.f, 1.f, 0.0f,			1.0f, 0.0f,				//v2
	0.0f, 1.0f, 0.0f,		1.f, 1.f, 0.0f,			0.0f, 1.0f,				//v3

	//BL
	//pos					//col			
	-1.0f, 0.0f, -1.0f,  	1.f, 0.0f, 1.f,			0.0f, 0.0f,				//v1
	-1.0f, 0.0f, 1.0f,      1.f, 0.0f, 1.f,			1.0f, 0.0f,				//v2
	0.0f, -1.0f, 0.0f,      1.f, 0.0f, 1.f,			0.0f, 1.0f,				//v3

	//BB
	//pos					//col			
	1.0f, 0.0f, -1.0f,      0.f, 1.f,  1.f,			0.0f, 0.0f,				//v1
	-1.0f, 0.0f, -1.0f,     0.f, 1.f,  1.f, 		1.0f, 0.0f,				//v2
	0.0f, -1.0f, 0.0f,      0.f, 1.f,  1.f, 		0.0f, 1.0f,				//v3

	//BR
	//pos					//col			
	1.0f, 0.0f, 1.0f,  	   0.5f, 0.5f, 0.f,			0.0f, 0.0f,				//v1
	1.0f, 0.0f, -1.0f,     0.5f, 0.5f, 0.f,			1.0f, 0.0f,				//v2
	0.0f, -1.0f, 0.0f,     0.5f, 0.5f, 0.f,			0.0f, 1.0f,				//v3

	//BF
	//pos					//col			
	-1.0f, 0.0f, 1.0f,  	0.5f, 0.0f, 0.0f,		0.0f, 0.0f,				//v1
	1.0f, 0.0f, 1.0f,       0.5f, 0.f,  0.0f,		1.0f, 0.0f,				//v2
	0.0f, -1.0f, 0.0f,      0.5f, 0.f,  0.0f,		0.0f, 1.0f,				//v3
};

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Handle key presses
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Using the arrow keys to move a camera around the object.
	float x_offset = 0.f;
	float y_offset = 0.f;
	bool cam_changed = false;
	float sentitivity = 1.f;

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		x_offset = 1.f * sentitivity;
		y_offset = 0.f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		x_offset = -1.f * sentitivity;
		y_offset = 0.f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		x_offset = 0.f;
		y_offset = 1.f * sentitivity;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		x_offset = 0.f;
		y_offset = -1.f * sentitivity;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		cam_dist -= 0.1f * sentitivity;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		cam_dist += 0.1f * sentitivity;
		cam_changed = true;
	}

	// Commit changes to camera.
	if (cam_changed) {
		MoveAndOrientCamera(Camera, glm::vec3(0.f, 0.f, 0.f), cam_dist, x_offset, y_offset);
	}
}

void ResizeCallback(GLFWwindow*, int w, int h)
{
	// Handle window resizes.
	// Area we want to render in in the window.
	// Coordinates and window size.
	glViewport(0, 0, w, h);
}

void initialise_buffers() {
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
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	// Enable the vertex attribute array
	glEnableVertexAttribArray(0);
	// Colour attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Unbind both by binding to 0
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main() {


	unsigned int width = 800;
	unsigned int height = 600;
	// Calculate number of vertices in the array
	int num_vertices = sizeof(vertices) / sizeof(vertices[0]) / 8;
	// Initialize GLFW
	glfwInit();

	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(width, height, "Assessment2", NULL, NULL);

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
	
	// Initialise the camera
	InitCamera(Camera);

	// Create VAO and VBOs
	initialise_buffers();

	// Create a uniform that will be used to set the texture.
	GLuint texUniform = glGetUniformLocation(program, "tex0");
	glUniform1i(texUniform, 0);

	// Texture to load.
	GLuint texture = setup_texture("bricks.jpg");

	// Account for depth of 3D objects.
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		// Clear the colour buffer and give another colour.
		glClearColor(0.07f, 0.31f, 0.17f, 1.0f);
		// Clear colour and depth buffer each iteration.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Use the given texture
		glBindTexture(GL_TEXTURE_2D, texture);
		// Actiate shader program
		glUseProgram(program);
		// Bind the VAO to tell gl we want to use this one
		glBindVertexArray(VAOs[0]);

		// Initialise space matrices.
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		// Rotate the model view.
		// Model to rotate, amount to rotate, axis to rotate around.
		model = glm::rotate(model, (float)glfwGetTime() / 20, glm::vec3(0.f, 1.f, 0.f));
		// Move the world view back and up.

		view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
		// Define a perspective that clips view at 0.1 near and 100 far.
		projection = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 100.0f);

		// Set the values in the vertex shader.
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// Draw the triangle
		// Type of primitive, starting value, amount of vertices.
		glDrawArrays(GL_TRIANGLES, 0, num_vertices);
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