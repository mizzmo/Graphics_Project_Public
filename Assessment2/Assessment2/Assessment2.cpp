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
#include "plane.h"
#include "object_parser.h"

#define NUM_VBO 3
#define NUM_VAO 3

SCamera Camera;

std::vector<GLfloat> planeVertices;
vector<GLfloat> ship_array;



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
	// Camera sensitivity.
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

vector<GLfloat> tri_to_fl_array(const std::vector<triangle>& triangles) {
	// Convert the triangle data to a flat array of floats.
	// Each triangle has 3 vertices, each vertex has 8 floats (x, y, z, r, g, b, s, t)
	// So each triangle has 3 * 8 = 24 floats.
	std::vector<GLfloat> tri_array(triangles.size() * 18);
	int index = 0;

	// Loop through each triangle and store its data in the array
	for (const triangle& tri : triangles) {
		tri_array[index++] = tri.v1.pos.x;
		tri_array[index++] = tri.v1.pos.y;
		tri_array[index++] = tri.v1.pos.z;
		tri_array[index++] = tri.v1.col.r;
		tri_array[index++] = tri.v1.col.g;
		tri_array[index++] = tri.v1.col.b;
		// You can implement textures here if needed

		tri_array[index++] = tri.v2.pos.x;
		tri_array[index++] = tri.v2.pos.y;
		tri_array[index++] = tri.v2.pos.z;
		tri_array[index++] = tri.v2.col.r;
		tri_array[index++] = tri.v2.col.g;
		tri_array[index++] = tri.v2.col.b;

		tri_array[index++] = tri.v3.pos.x;
		tri_array[index++] = tri.v3.pos.y;
		tri_array[index++] = tri.v3.pos.z;
		tri_array[index++] = tri.v3.col.r;
		tri_array[index++] = tri.v3.col.g;
		tri_array[index++] = tri.v3.col.b;
	}
	for (int i = 0; i < triangles.size() * 18; ++i) {
		std::cout << tri_array[i] << " ";
	}
	return tri_array;
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
	// Bind VAO 0 (for the triangle)
	glBindVertexArray(VAOs[0]);
	// Generate number of VBOs
	glCreateBuffers(NUM_VBO, VBOs);
	// Bind VBO 0 (for the triangle vertices)
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	// Store vertices in VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Configure VAO 0 (triangle)
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Colour attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Bind VAO 1 (for the plane)
	glBindVertexArray(VAOs[1]);
	// Bind VBO 1 (for the plane vertices)
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	// Generate the vertices for the plane.
	planeVertices = generate_plane(10, 1.0f);
	glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof(float), planeVertices.data(), GL_STATIC_DRAW);
	// Position Attribute for the plane
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); 

	// Colour Attribute for the plane
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Configure VAO 2 (object 1)
	glBindVertexArray(VAOs[2]);
	// Bind VBO 1 (for the plane vertices)
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);	
	// Load an Object
	std::vector<triangle> triangles;
	std::string obj_path = "objs/ufo/Low_poly_UFO.obj";
	// Specify the base folder path
	std::string base_path = "objs/ufo/";
	int num_triangles = obj_parse(obj_path.c_str(), &triangles, base_path.c_str());
	// Convert to array of floats
	ship_array = tri_to_fl_array(triangles);
	// Upload the vertex data to the VBO
	glBufferData(GL_ARRAY_BUFFER, ship_array.size() * sizeof(GLfloat), ship_array.data(), GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Colour attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// Unbind buffers and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main() {
	unsigned int width = 800;
	unsigned int height = 600;
	// Calculate number of vertices in the array
	int num_vertices = sizeof(vertices) / (8 * sizeof(float));
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
	GLuint colour_program = CompileShader("vertex_shader.vert", "col_fragment_shader.frag");
	
	// Initialise the camera
	InitCamera(Camera);
	cam_dist = 0.5f;
	MoveAndOrientCamera(Camera, glm::vec3(0.f, 0.f, 0.f), cam_dist, 0.f, 0.f);

	// Create VAO and VBOs
	initialise_buffers();

	// Texture to load.
	GLuint brick_tex = setup_texture("bricks.jpg");
	GLuint grass_tex = setup_texture("grass.jpg");

	// Account for depth of 3D objects.
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		// Clear the colour buffer
		glClearColor(0.07f, 0.31f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use the shader program
		glUseProgram(program);

		// --- Draw the Triangle ---
		// Bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, brick_tex);
		glUniform1i(glGetUniformLocation(program, "tex0"), 0);

		glBindVertexArray(VAOs[0]);

		glm::mat4 modelTriangle = glm::mat4(1.0f);
		modelTriangle = glm::scale(modelTriangle, glm::vec3(0.1f, 0.1f, 0.1f));
		modelTriangle = glm::rotate(modelTriangle, (float)glfwGetTime() / 20, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelTriangle));

		glm::mat4 view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		int num_vertices = sizeof(vertices) / (8 * sizeof(float));
		glDrawArrays(GL_TRIANGLES, 0, num_vertices);

		// --- Draw the Plane ---
		// Bind texture to plane
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, grass_tex);
		glUniform1i(glGetUniformLocation(program, "tex1"), 1);

		glBindVertexArray(VAOs[1]);

		glm::mat4 modelPlane = glm::mat4(1.0f);
		modelPlane = glm::translate(modelPlane, glm::vec3(-0.5f, -0.1f, 0.5f));
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelPlane));
		// Use the same view and projection matrices as the triangle 
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		int num_plane_vertices = planeVertices.size() / 5;
		glDrawArrays(GL_TRIANGLES, 0, num_plane_vertices);


		// --- Draw the Object ---
		// Switch to basic colour shader
		glUseProgram(colour_program);
		// Bind the VAO
		glBindVertexArray(VAOs[2]);  

		// Apply transformations to the object
		glm::mat4 modelObject = glm::mat4(1.0f);
		modelObject = glm::scale(modelObject, glm::vec3(0.1f, 0.1f, 0.1f));
		modelObject = glm::translate(modelObject, glm::vec3(-0.5f, -0.1f, 0.5f));
		glUniformMatrix4fv(glGetUniformLocation(colour_program, "model"), 1, GL_FALSE, glm::value_ptr(modelObject));

		// Use the same view and projection matrices as before
		glUniformMatrix4fv(glGetUniformLocation(colour_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(colour_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		int num_object_vertices = ship_array.size() / 6;
		glDrawArrays(GL_TRIANGLES, 0, num_object_vertices);

		glBindVertexArray(0);
		glfwSwapBuffers(window);
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