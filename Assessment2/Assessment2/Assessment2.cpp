

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
#include "shadow.h"
#include "cylinder.h"


// useful for picking colours
// https://keiwando.com/color-picker/

#define NUM_VBO 5
#define NUM_VAO 5

unsigned int width = 800;
unsigned int height = 600;

// Interactive Cameras
SCamera Model_Viewer_Camera;
SCamera Fixed_Rotate_Camera;

// Static Cameras
SCamera First_Person_Camera;
SCamera Fixed_Left_Front;
SCamera Fixed_Left_Rear;
SCamera Fixed_Right_Front;
SCamera Fixed_Right_Rear;

int current_camera = 0;
const int  num_cameras = 7;

SCamera* cameras[num_cameras] = { &Model_Viewer_Camera, &First_Person_Camera, &Fixed_Rotate_Camera, &Fixed_Left_Front, &Fixed_Left_Rear, &Fixed_Right_Front, &Fixed_Right_Rear };

SCamera* activeCamera = &Model_Viewer_Camera;

// Radius of the cam orbit
float orbit_radius = 10.0f;
// Speed of orbit
float orbit_speed = 0.00005f; 
// Center of the orbit
glm::vec3 orbit_center(0.0f, 0.0f, 0.0f);

glm::vec3 lightDirection = glm::vec3(-0.6f, -0.5f, 0.6f);
glm::vec3 lightPos = glm::vec3(10.f, 9.f, -9.f);

// Shadows
#define SH_MAP_WIDTH 2048
#define SH_MAP_HEIGHT 2048


//std::vector<GLfloat> planeVertices;
vector<GLfloat> ship_array;
std::vector<GLfloat> cylinder;


// Textures for UFO
std::vector<GLuint> ufo_texture_ids;

GLuint brick_tex, sand_tex, ship_tex, ship_glow, ship_normal, ship_specular, ship_bump;

// Mouse Callback
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
bool firstMouse = true;
// Difference in time between frames
float deltaTime = 0.0f; 
float lastFrame = 0.0f; 

// Create a Vertex Array Object (VAO)
// Stores pointers to VBOs and tells OpenGL how to interpret the data.
// Need to make before VBO, can be used to switch between them
GLuint VAOs[NUM_VAO];
// Store vertex data in vertex buffer object.
GLuint VBOs[NUM_VBO];

// Create an array of floats representing coordinates of a triangle.
GLfloat vertices[] = {
	// Coordinates, Colours, Texture, Normal
	-1.0f, 0.0f, -1.0f,  	1.f, 1.0f, 1.0f,		0.0f, 0.0f,		-1.f, 1.f, 0.f,		//v1
	-1.0f, 0.0f, 1.0f,      1.f, 1.0f, 1.0f,		1.0f, 0.0f,		-1.f, 1.f, 0.f,		//v2
	0.0f, 1.0f, 0.0f,		1.f, 1.0f, 1.0f,		0.0f, 1.0f,		-1.f, 1.f, 0.f,		//v3

	//TB
	//pos					//col			
	1.0f, 0.0f, -1.0f,		1.f, 1.0f, 1.0f,			0.0f, 0.0f,		0.f, 1.f, -1.f,		//v1
	-1.0f, 0.0f, -1.0f,		1.f, 1.0f, 1.0f, 		1.0f, 0.0f,		0.f, 1.f, -1.f,		//v2
	0.0f, 1.0f, 0.0f,		1.f, 1.0f, 1.0f, 		0.0f, 1.0f,		0.f, 1.f, -1.f,		//v3

	//TR
	//pos					//col			
	1.0f, 0.0f, 1.0f,  		1.f, 1.0f, 1.0f,		0.0f, 0.0f,		1.f, 1.f, 0.f,		//v1
	1.0f, 0.0f, -1.0f,		1.f, 1.0f, 1.0f,		1.0f, 0.0f,		1.f, 1.f, 0.f,		//v2
	0.0f, 1.0f, 0.0f,		1.f, 1.0f, 1.0f,		0.0f, 1.0f,		1.f, 1.f, 0.f,		//v3

	//TF
	//pos					//col			
	-1.0f, 0.0f, 1.0f,  	1.f, 1.0f, 1.0f,			0.0f, 0.0f,		0.f, 1.f, 1.f,		//v1
	1.0f, 0.0f, 1.0f,		1.f, 1.0f, 1.0f,			1.0f, 0.0f,		0.f, 1.f, 1.f,		//v2
	0.0f, 1.0f, 0.0f,		1.f, 1.0f, 1.0f,			0.0f, 1.0f,		0.f, 1.f, 1.f,		//v3

	//BL
	//pos					//col			
	-1.0f, 0.0f, -1.0f,  	1.f, 1.0f, 1.0f,			0.0f, 0.0f,		-1.f, -1.f, 0.f,		//v1
	-1.0f, 0.0f, 1.0f,      1.f, 1.0f, 1.0f,			1.0f, 0.0f,		-1.f, -1.f, 0.f,		//v2
	0.0f, -1.0f, 0.0f,      1.f, 1.0f, 1.0f,			0.0f, 1.0f,		-1.f, -1.f, 0.f,		//v3

	//BB
	//pos					//col			
	1.0f, 0.0f, -1.0f,      1.f, 1.0f, 1.0f,			0.0f, 0.0f,		0.f, -1.f, -1.f,		//v1
	-1.0f, 0.0f, -1.0f,     1.f, 1.0f, 1.0f, 		1.0f, 0.0f,		0.f, -1.f, -1.f,		//v2
	0.0f, -1.0f, 0.0f,      1.f, 1.0f, 1.0f, 		0.0f, 1.0f,		0.f, -1.f, -1.f,		//v3

	//BR
	//pos					//col			
	1.0f, 0.0f, 1.0f,  	    1.f, 1.0f, 1.0f,			0.0f, 0.0f,		1.f, -1.f, 0.f,		//v1
	1.0f, 0.0f, -1.0f,      1.f, 1.0f, 1.0f,			1.0f, 0.0f,		1.f, -1.f, 0.f,		//v2
	0.0f, -1.0f, 0.0f,      1.f, 1.0f, 1.0f,			0.0f, 1.0f,		1.f, -1.f, 0.f,		//v3

	//BF
	//pos					//col			
	-1.0f, 0.0f, 1.0f,  	1.f, 1.0f, 1.0f,		0.0f, 0.0f,		0.f, -1.f, 1.f,		//v1
	1.0f, 0.0f, 1.0f,       1.f, 1.0f, 1.0f,		1.0f, 0.0f,		0.f, -1.f, 1.f,		//v2
	0.0f, -1.0f, 0.0f,      1.f, 1.0f, 1.0f,		0.0f, 1.0f,		0.f, -1.f, 1.f,		//v3
};


GLfloat flat_square[] = {
	// X,   Y,   Z,      R,  G,  B,      U,   V,      Nx,  Ny, Nz
	// Top face
	-0.5f, 0.0f, 0.5f, 1.f, 1.f, 1.f, 0.0f, 10.0f, 0.f, 1.f, 0.f,
	0.5f, 0.0f, 0.5f, 1.f, 1.f, 1.f, 10.0f, 10.0f, 0.f, 1.f, 0.f,
	0.5f, 0.0f, -0.5f, 1.f, 1.f, 1.f, 10.0f, 0.0f, 0.f, 1.f, 0.f,

	-0.5f, 0.0f, 0.5f, 1.f, 1.f, 1.f, 0.0f, 10.0f, 0.f, 1.f, 0.f,
	0.5f, 0.0f, -0.5f, 1.f, 1.f, 1.f, 10.0f, 0.0f, 0.f, 1.f, 0.f,
	-0.5f, 0.0f, -0.5f, 1.f, 1.f, 1.f, 0.0f, 0.0f, 0.f, 1.f, 0.f,
};


void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Handle key presses
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		lightDirection = activeCamera->Front;
		printf("Light Direction: %f %f %f\n", lightDirection.x, lightDirection.y, lightDirection.z);
		printf("Camera Position: %f %f %f\n", activeCamera->Position.x, activeCamera->Position.y, activeCamera->Position.z);
	}
		
		
	
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		// Switch between cameras
		if (current_camera+1 == num_cameras) {
			// Loop back to start if on last camera
			current_camera = 0;
		}
		else {
			// Move to next camera.
			current_camera++;
		}
		printf("Changed Camera to Camera %d\n", current_camera);
		// Switch to the correct camera
		activeCamera = cameras[current_camera];
	}
	// Model Viewer Camera Logic
	if (current_camera == 0) {
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
			float potential_pitch = Model_Viewer_Camera.Pitch - y_offset * Model_Viewer_Camera.MovementSpeed;
			if (potential_pitch < 0.f) {
				// Prevent the camera from looking below the plane.
				y_offset = 0.f;
			}
			MoveAndOrientCamera(Model_Viewer_Camera, glm::vec3(0.f, 0.f, 0.f), cam_dist, x_offset, y_offset);

		}
	}
	// Rotational Camera Orbit Speed Controls
	if (current_camera == 2) {
		if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS)
			orbit_speed += 0.001f;
		if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
			orbit_speed -= 0.001f;
	}
	
}

void ProcessInput(GLFWwindow* window, SCamera& camera, float deltaTime)
{
	if (current_camera == 1) {
		float velocity = camera.MovementSpeed * deltaTime;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.Position += camera.Front * velocity;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.Position -= camera.Front * velocity;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.Position -= camera.Right * velocity;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.Position += camera.Right * velocity;
		// Prevent from going below the ground
		if (camera.Position.y < 0.1f)
			camera.Position.y = 0.1f;
	}

}

vector<GLfloat> tri_to_fl_array(const std::vector<triangle>& triangles) {
	// Each triangle has 3 vertices, each vertex has 12 floats
	// (x, y, z, r, g, b, nx, ny, nz, s, t) -> 3 * 11 = 33 floats per triangle
	std::vector<GLfloat> tri_array(triangles.size() * 36);
	int index = 0;

	for (const triangle& tri : triangles) {
		const vertex* verts[3] = { &tri.v1, &tri.v2, &tri.v3 };

		for (int i = 0; i < 3; ++i) {
			const vertex& v = *verts[i];
			// Position
			tri_array[index++] = v.pos.x;
			tri_array[index++] = v.pos.y;
			tri_array[index++] = v.pos.z;
			// Color
			tri_array[index++] = v.col.r;
			tri_array[index++] = v.col.g;
			tri_array[index++] = v.col.b;
			// Texture coordinates
			tri_array[index++] = v.tex.x;
			tri_array[index++] = v.tex.y;
			// Normal
			tri_array[index++] = v.nor.x;
			tri_array[index++] = v.nor.y;
			tri_array[index++] = v.nor.z;
		}
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

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	// Only process mouse input if the current active camera is this one
	if (current_camera != 1) {
		return;
	}

	// On first mouse event init last mouse position to avoid jump
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// Calculate movement since last frame
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	// Update last pos
	lastX = xpos;
	lastY = ypos;

	// Apply offset
	OrientFirstPersonCamera(*activeCamera, xoffset, yoffset);
}

void initialise_buffers() {
	// Generate number of VAOs
	glGenVertexArrays(NUM_VAO, VAOs);

	// ---------- PYRAMID ----------
	// Bind VAO 0 (for the triangle)
	glBindVertexArray(VAOs[0]);
	// Generate number of VBOs
	glCreateBuffers(NUM_VBO, VBOs);
	// Bind VBO 0 (for the triangle vertices)
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	// Store vertices in VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Colour attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// Normal attribute
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);


	//  ---------- UFO ----------
	// Configure VAO 2 (UFO)
	glBindVertexArray(VAOs[2]);
	// Bind VBO 2 (for the UFO vertices)
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);	

	// Create a texture map to store textures
	std::map<std::string, GLuint> textures;

	// Load an Object with textures
	std::vector<triangle> triangles;
	// Specify the base folder path
	std::string obj_path = "objs/ufo/Low_poly_UFO.obj";
	std::string base_path = "objs/ufo";
	int num_triangles = obj_parse(obj_path.c_str(), &triangles, base_path.c_str(), &textures);

	// Convert to array of floats
	ship_array = tri_to_fl_array(triangles);
	// Upload the vertex data to the VBO
	glBufferData(GL_ARRAY_BUFFER, ship_array.size() * sizeof(GLfloat), ship_array.data(), GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Colour attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// Normal attribute
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// Store texture IDs for rendering
	for (const auto& triangle : triangles) {
		if (triangle.texID > 0 && std::find(ufo_texture_ids.begin(), ufo_texture_ids.end(), triangle.texID) == ufo_texture_ids.end()) {
			ufo_texture_ids.push_back(triangle.texID);
		}
	}


	// ---- FLAT PLANE ----
	glBindVertexArray(VAOs[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]);

	// Generate the vertices for the plane.
	glBufferData(GL_ARRAY_BUFFER, sizeof(flat_square), flat_square, GL_STATIC_DRAW);
	// Position Attribute for the plane
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Colour Attribute for the plane
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture attribute for plane.
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// Normal attribute for plane.
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);


	// ---- Cylinder ----
	int num_cylinder_vertices = 0;
	// Use 48 Faces
	// Num Faces, Top Radius, Bottom Radius, Height, Opacity, Store num vertices
	cylinder = form_cylinder(64, 0.3f, 1.8f, 4.f, 0.3f, num_cylinder_vertices);

	glBindVertexArray(VAOs[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[4]);

	glBufferData(GL_ARRAY_BUFFER, cylinder.size() * sizeof(GLfloat), cylinder.data(), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Colour attribute & Transparency using Alpha val
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// Normal attribute
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);


	// Unbind buffers and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void initialise_cameras() {
	InitCamera(Model_Viewer_Camera);
	cam_dist = 9.f;
	int fixed_dist = 9.f;
	// Initial position far view of entire scene, slight upward angle
	MoveAndOrientCamera(Model_Viewer_Camera, glm::vec3(0.f, 0.f, 0.f), cam_dist, 0.f, -20.f);

	// Initialise first person camera
	InitFirstPersonCamera(First_Person_Camera);

	InitCamera(Fixed_Left_Front);
	// Initial position left front corner
	MoveAndOrientCamera(Fixed_Left_Front, glm::vec3(0.f, 0.f, 0.f), fixed_dist, 45.f, -20.f);

	InitCamera(Fixed_Left_Rear);
	// Initial position left rear corner
	MoveAndOrientCamera(Fixed_Left_Rear, glm::vec3(0.f, 0.f, 0.f), fixed_dist, 90.f, -20.f);

	InitCamera(Fixed_Right_Front);
	// Initial position right front corner
	MoveAndOrientCamera(Fixed_Right_Front, glm::vec3(0.f, 0.f, 0.f), fixed_dist, -45.f, -20.f);

	InitCamera(Fixed_Right_Rear);
	// Initial position right rear corner
	MoveAndOrientCamera(Fixed_Right_Rear, glm::vec3(0.f, 0.f, 0.f), fixed_dist, -90.f, -20.f);

	
	InitCamera(Fixed_Rotate_Camera);
	// Initial position far view with slight x and y offset
	MoveAndOrientCamera(Fixed_Rotate_Camera, glm::vec3(0.f, 0.f, 0.f), cam_dist, -15.f, -20.f);
}

void draw_cylinder(int program) {
	glBindVertexArray(VAOs[4]);

	// Apply transformations
	glm::mat4 modelCylinder = glm::mat4(1.0f);
	modelCylinder = glm::scale(modelCylinder, glm::vec3(1.f, 1.f, 1.f));
	modelCylinder = glm::translate(modelCylinder, glm::vec3(0.f, 1.f, 0.f));
	// Rotate in the Y to be upright
	modelCylinder = glm::rotate(modelCylinder, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelCylinder));

	int num_object_vertices = cylinder.size() / 11;
	// Draw cylinder
	glDrawArrays(GL_TRIANGLES, 0, num_object_vertices);
}

void draw_ufo(unsigned int program) {
	// Bind the VAO
	glBindVertexArray(VAOs[2]);

	// Apply transformations to the UFO
	glm::mat4 modelUFO = glm::mat4(1.0f);
	modelUFO = glm::scale(modelUFO, glm::vec3(0.06f, 0.06f, 0.06f));
	modelUFO = glm::translate(modelUFO, glm::vec3(0.f, 35.f, 0.f));
	modelUFO = glm::rotate(modelUFO, (float)glfwGetTime() / 20, glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelUFO));


	int num_object_vertices = ship_array.size() / 11;
	// Draw the UFO
	glDrawArrays(GL_TRIANGLES, 0, num_object_vertices);

}

void draw_flat_plane(unsigned int program) {
	// Flat Plane
	glBindVertexArray(VAOs[3]);
	glm::mat4 modelFlatPlane = glm::mat4(1.0f);
	modelFlatPlane = glm::translate(modelFlatPlane, glm::vec3(0.f, 0.f, 0.f));
	modelFlatPlane = glm::scale(modelFlatPlane, glm::vec3(15.f, 15.f, 15.f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelFlatPlane));
	glDrawArrays(GL_TRIANGLES, 0, sizeof(flat_square) / (11 * sizeof(float)));
}

void draw_pyramid(unsigned int program) {
	// Pyramid
	glBindVertexArray(VAOs[0]);
	glm::mat4 modelTriangle = glm::mat4(1.0f);
	modelTriangle = glm::scale(modelTriangle, glm::vec3(1.f, 1.f, 1.f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelTriangle));
	int num_vertices = sizeof(vertices) / (11 * sizeof(float));
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);
}

void generateDepthMap(unsigned int shadowShaderProgram, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix) {
	// Set the viewport to the size of the shadow map
	glViewport(0, 0, SH_MAP_WIDTH, SH_MAP_HEIGHT);

	// Bind the framebuffer for the shadow map
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);

	// Clear the depth buffer of the framebuffer
	glClear(GL_DEPTH_BUFFER_BIT);

	// Use the shadow shader program
	glUseProgram(shadowShaderProgram);

	// Set the uniform for the light space matrix in the shader
	glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));

	// Draw the floor and cubes using the shadow shader
	draw_pyramid(shadowShaderProgram);
	draw_ufo(shadowShaderProgram);
	draw_cylinder(shadowShaderProgram);
	draw_flat_plane(shadowShaderProgram);


	// Unbind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderWithShadow(unsigned int renderShaderProgram, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix) {
	glViewport(0, 0, width, height);

	static const GLfloat bgd[] = { .8f, .8f, .8f, 1.f };
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(renderShaderProgram);

	// Activate and Bind shadow map to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadow.Texture);
	glUniform1i(glGetUniformLocation(renderShaderProgram, "shadowMap"), 0);

	// Activate brick texture in texture unit 2
	glActiveTexture(GL_TEXTURE2);
	glUniform1i(glGetUniformLocation(renderShaderProgram, "tex0"), 2);

	// For the regular objects, set to false
	glUniform1i(glGetUniformLocation(renderShaderProgram, "uses_specular"), false);
	glUniform1i(glGetUniformLocation(renderShaderProgram, "uses_glow"), false);
	glUniform1i(glGetUniformLocation(renderShaderProgram, "uses_normal"), false);
	glUniform1i(glGetUniformLocation(renderShaderProgram, "uses_bump"), false);


	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightColour"), 1.f, 0.98f, 0.7f);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "camPos"), activeCamera->Position.x, activeCamera->Position.y, activeCamera->Position.z);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glm::mat4 view = glm::mat4(1.f);
	view = glm::lookAt(activeCamera->Position, activeCamera->Position + activeCamera->Front, activeCamera->Up);
	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::mat4(1.f);
	projection = glm::perspective(glm::radians(45.f), (float)width / (float)height, .01f, 100.f);
	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	draw_pyramid(renderShaderProgram);

	// ---- Flat Plane ----
	// Activate sand texture in texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(renderShaderProgram, "tex0"), 1);
	draw_flat_plane(renderShaderProgram);

	// --- Cylinder ---
	// Deactivate textures
	glUniform1i(glGetUniformLocation(renderShaderProgram, "uses_texture"), false);


	draw_cylinder(renderShaderProgram);
	// Re-activate textures
	glUniform1i(glGetUniformLocation(renderShaderProgram, "uses_texture"), true);


	// ---- UFO ----
	// Activate textures for UFO
	// Texture Unit 3 - UFO Diffuse
	glActiveTexture(GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(renderShaderProgram, "tex0"), 3);
	// Texture Unit 4 - UFO Glow
	glActiveTexture(GL_TEXTURE4);
	glUniform1i(glGetUniformLocation(renderShaderProgram, "glow_map"), 4);
	// Texture unit 5 - UFO Normal
	glActiveTexture(GL_TEXTURE5);
	glUniform1i(glGetUniformLocation(renderShaderProgram, "normal_map"), 5);
	// Texture unit 6 - UFO Specular
	glActiveTexture(GL_TEXTURE6);
	glUniform1i(glGetUniformLocation(renderShaderProgram, "specular_map"), 6);
	// Texture unit 7 - UFO Bump
	glActiveTexture(GL_TEXTURE7);
	glUniform1i(glGetUniformLocation(renderShaderProgram, "bump_map"), 7);

	// Activate Maps in Fragment Shader
	// Activate the glow map
	glUniform1i(glGetUniformLocation(renderShaderProgram, "uses_glow"), true);
	// Activate the normal map
	glUniform1i(glGetUniformLocation(renderShaderProgram, "uses_normal"), true);
	// Activate specular map
	glUniform1i(glGetUniformLocation(renderShaderProgram, "uses_specular"), true);
	// Set a scale for the bump map
	glUniform1i(glGetUniformLocation(renderShaderProgram, "bump_scale"), 0.1f);

	draw_ufo(renderShaderProgram);

}

int main() {
	// Initialize GLFW
	glfwInit();

	// Enable 8x Multisampling
	glfwWindowHint(GLFW_SAMPLES, 8);

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
	// Tracks mouse cursor
	glfwSetCursorPosCallback(window, MouseCallback);
	// Make cursor invisible
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	

	// Init gl3w after making context.
	if (gl3wInit()) {
		std::cout << "Failed to initialize GL3W" << std::endl;
		return -1;
	}

	// Get the maximum amount of samples allowed by querying openGL
	int max_samples;
	glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
	printf("Max Samples Supported: %d\n", max_samples);

	ShadowStruct shadow = setup_shadowmap(SH_MAP_WIDTH, SH_MAP_HEIGHT);

	// Create a vertext shader and Fragment Shader using the Shader class.
	GLuint lighting_program = CompileShader("lighting_vertex.vert", "lighting_fragment.frag");
	GLuint shadow_shader = CompileShader("shadow.vert", "shadow.frag");
	
	// Initialise the cameras
	initialise_cameras();

	// Create VAO and VBOs
	initialise_buffers();

	// Texture to load.
	brick_tex = setup_texture("bricks.jpg");
	sand_tex = setup_texture("sand.jpg");
	// Texture for the UFO object.
	ship_tex = setup_texture("objs/ufo/ufo_diffuse.png");
	ship_glow = setup_texture("objs/ufo/ufo_diffuse_glow.png");
	ship_normal = setup_texture("objs/ufo/ufo_normal.png");
	ship_specular = setup_texture("objs/ufo/ufo_spec.png");
	ship_bump = setup_texture("objs/ufo/Map__7_Normal_Bump.tga");

	// Enable blending for transparency
	glEnable(GL_BLEND);
	// Specify blend function 
	// Final colour is combination of source fragment scaled by alpha and destination fragment scaled by 1 - alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Texture Unit 0 is used for Shadow Mapping.

	// Bind to texture units
	// Texture unit 1 - Sand
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sand_tex);

	// Texture unit 2 - Bricks
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, brick_tex);

	// Texture unit 3 - UFO Diffuse
	glActiveTexture(GL_TEXTURE3); 
	glBindTexture(GL_TEXTURE_2D, ship_tex);

	// Texture unit 4 - UFO Glow
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, ship_glow);

	// Texture unit 5 - UFO Normal
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, ship_normal);

	// Texture unit 6 - UFO Specular
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, ship_specular);

	// Texture unit 7 - UFO Bump
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, ship_bump);


	// Account for depth of 3D objects.
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		// Clear the colour buffer
		glClearColor(0.01f, 0.01f, 0.27f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use the shader program
		glUseProgram(lighting_program);

		glEnable(GL_MULTISAMPLE);

		// Create a Projected Light Space Matrix
		// Adjust these parameters based on your scene size
		float near_plane = 0.1f, far_plane = 20.0f;
		// Use tighter bounds that just cover your scene
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

		// Make sure light position is far enough away in the direction opposite to light direction
		lightPos = -lightDirection * 10.0f; 
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projectedLightSpaceMatrix = lightProjection * lightView;


		generateDepthMap(shadow_shader, shadow, projectedLightSpaceMatrix);
		// Render the pyramid with a shadow map
		renderWithShadow(lighting_program, shadow, projectedLightSpaceMatrix);

		// First person camera
		if (current_camera == 1) {
			// Update the frame count
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			ProcessInput(window, *activeCamera, deltaTime);
		}
		// Orbiting camera
		if (current_camera == 2) {

			// Use time to manage rotation
			float time = (float)glfwGetTime();

			float xoffset = time * orbit_speed;
			float yoffset = 0.0f;

			// Apply the translation 
			MoveAndOrientCamera(Fixed_Rotate_Camera, orbit_center, orbit_radius, xoffset, yoffset);
		}

		glBindVertexArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	// Remove objects
	glDeleteVertexArrays(NUM_VAO, VAOs);
	glDeleteBuffers(NUM_VBO, VBOs);
	// Delete the shader program
	glDeleteProgram(lighting_program);

	// Remove the window
	glfwDestroyWindow(window);
	// Terminate GLFW
	glfwTerminate();
	return 0;
}