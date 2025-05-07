

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
#include "casteljau.h"
#include "interactivity.h"


// useful for picking colours
// https://keiwando.com/color-picker/



unsigned int width = 800;
unsigned int height = 600; 
float fov = 45.f;

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

// UFO animation
float start_time = 0.f;
bool ufo_animation = false;
bool beam_animation = false;

// Directional Light
glm::vec3 lightDirection = glm::vec3(-0.6f, -0.5f, 0.6f);
glm::vec3 lightPos = glm::vec3(10.f, 9.f, -9.f);

// Spot light
#define NUM_SPOTLIGHTS 4
glm::vec3 spotLightDirections[NUM_SPOTLIGHTS];
glm::vec3 spotLightPos = glm::vec3(0.f, 1.f, 0.f);

// Shadows
#define SH_MAP_WIDTH 2048
#define SH_MAP_HEIGHT 2048


//std::vector<GLfloat> planeVertices;
std::vector<GLfloat> ship_array;
std::vector<GLfloat> cylinder;
std::vector<GLfloat> jet_array;
std::vector<GLfloat> desert_dunes;
std::vector<GLfloat> shooting_star;

// Textures for UFO
std::vector<GLuint> ufo_texture_ids;
// Textures for Jet
std::vector<GLuint> jet_texture_ids;

GLuint brick_tex, sand_tex, ship_tex, ship_glow, ship_normal, ship_specular, ship_bump, jet_tex, skybox_tex;


// Copy of matrices used by UFO
glm::mat4 projection;
glm::mat4 view;
// Determine if the UFO has been clicked
bool is_clicked = false;

// Mouse Callback
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
bool firstMouse = true;
// Difference in time between frames
float deltaTime = 0.0f; 
float lastFrame = 0.0f; 


#define NUM_VBO 8
#define NUM_VAO 8
// Create a Vertex Array Object (VAO)
// Stores pointers to VBOs and tells OpenGL how to interpret the data.
// Need to make before VBO, can be used to switch between them
GLuint VAOs[NUM_VAO];
// Store vertex data in vertex buffer object.
GLuint VBOs[NUM_VBO];

// Create an array of floats representing coordinates of a triangle.
GLfloat double_pyramid_vertices[] = {
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

GLfloat cube_vertices[] = {
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Handle key presses
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		lightDirection = activeCamera->Front;
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		// Where the UFO is in the scene
		glm::vec3 ship_pos = glm::vec3(0.f, 3.f, 0.f);

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glm::vec3 rayDir = screenToWorldRay(xpos, ypos, width, height, projection, view);
		// Radius of detection
		float pickRadius = 1.5f; 
		if (rayIntersectsSphere(activeCamera->Position, rayDir, ship_pos, pickRadius)) {
			std::cout << "UFO clicked!\n";
			is_clicked = true;
			if (!ufo_animation) {
				ufo_animation = true;
				beam_animation = true;
				start_time = glfwGetTime();
			}
		}
	}
}

void process_input(GLFWwindow* window, SCamera& camera, float deltaTime)
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
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		// Reset the click state for UFO position to reset
		is_clicked = false;
		ufo_animation = false;
		beam_animation = false;
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

void resize_callback(GLFWwindow*, int w, int h)
{
	// Update width and height values
	width = w;
	height = h;

	glViewport(0, 0, w, h);
	// Get new aspect ratio
	float aspect = (float)width / (float)height;
	// Update projection matrix with new widow size
	projection = glm::perspective(glm::radians(fov), aspect, 0.01f, 100.0f);
}

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	// Only process mouse input if the current active camera is this one
	if (current_camera == 1) {
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(double_pyramid_vertices), double_pyramid_vertices, GL_STATIC_DRAW);
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

	// Number of Squares, Width of each square
	desert_dunes = generate_plane(64, 30.f, glm::vec3(1.f, 0.f, 0.f));
	// Generate the vertices for the plane.
	glBufferData(GL_ARRAY_BUFFER, desert_dunes.size() * sizeof(GLfloat), desert_dunes.data(), GL_STATIC_DRAW);
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


	// ---- CYLINDER ----
	int num_cylinder_vertices = 0;
	// Use 48 Faces
	// Num Faces, Top Radius, Bottom Radius, Height, Opacity, Store num vertices
	cylinder = form_cylinder(64, 0.3f, 1.8f, 2.2f, 0.3f, num_cylinder_vertices);

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


	

	//  ---------- JET PLANE ----------
	// Configure VAO 5 (Plane)
	glBindVertexArray(VAOs[5]);
	// Bind VBO 5 (for the Plane vertices)
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[5]);

	// Create a texture map to store textures
	std::map<std::string, GLuint> jet_textures;

	// Load an Object with textures
	std::vector<triangle> jet_triangles;
	// Specify the base folder path
	obj_path = "objs/jet/Rafale.obj";
	base_path = "objs/jet";
	num_triangles = obj_parse(obj_path.c_str(), &jet_triangles, base_path.c_str(), &jet_textures);

	// Convert to array of floats
	jet_array = tri_to_fl_array(jet_triangles);
	// Upload the vertex data to the VBO
	glBufferData(GL_ARRAY_BUFFER, jet_array.size() * sizeof(GLfloat), jet_array.data(), GL_STATIC_DRAW);
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
	for (const auto& triangle : jet_triangles) {
		if (triangle.texID > 0 && std::find(jet_texture_ids.begin(), jet_texture_ids.end(), triangle.texID) == jet_texture_ids.end()) {
			jet_texture_ids.push_back(triangle.texID);
		}
	}


	// ---- SHOOTING STARS ----
	// Configure VAO 6
	glBindVertexArray(VAOs[6]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[6]);

	// Define control points
	std::vector<point> ctrl_points = {
			point(-5.f, 4.f, 0.f),
			point(0.f, 2.f, 0.f),
			point(-5.f, -4.f, 0.f),
			point(-2.f, -3.f, 0.f),
			point(-4.f, 2.f, 0.f),
			point(-5.f, -2.f, 0.f),
			point(5.f, 4.f, 0.f),
	};

	// Create curve points for the shooting star
	std::vector<point> points;
	points = EvaluateBezierCurve(ctrl_points, 128);

	// Create the curve vector
	int num_curve_verts = 0;
	int num_curve_floats = 0;
	shooting_star.clear();
	shooting_star = MakeFloatsFromVector(points, 1.f, 1.f, 0.f);
	// Upload the vertex data to the VBO
	glBufferData(GL_ARRAY_BUFFER, shooting_star.size() * sizeof(GLfloat), shooting_star.data(), GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Colour attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// ---- SKYBOX ----
	glBindVertexArray(VAOs[7]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[7]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices) * sizeof(GLfloat), cube_vertices, GL_STATIC_DRAW);

	// Position attribute (Dobules as texture coordinates for cube map)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

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

float random_range(float min, float max) {
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void update_shooting_star_path() {
	// Randomize control points (example: stay within view bounds)
	std::vector<point> ctrl_points;
	ctrl_points.push_back(point(random_range(-6.f, 6.f), random_range(4.f, 6.f), 0.f));
	ctrl_points.push_back(point(random_range(-6.f, 6.f), random_range(0.f, 3.f), 0.f));
	ctrl_points.push_back(point(random_range(-6.f, 6.f), random_range(-3.f, -1.f), 0.f));
	ctrl_points.push_back(point(random_range(-6.f, 6.f), random_range(-4.f, -2.f), 0.f));

	std::vector<point> points = EvaluateBezierCurve(ctrl_points, 128);

	shooting_star.clear();
	shooting_star = MakeFloatsFromVector(points, 1.f, 1.f, 0.f);

	// Update VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[6]);
	glBufferData(GL_ARRAY_BUFFER, shooting_star.size() * sizeof(GLfloat), shooting_star.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_star(unsigned int program) {
    glUseProgram(program);

	glm::mat4 view = glm::lookAt(activeCamera->Position, activeCamera->Position + activeCamera->Front, activeCamera->Up);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)width / (float)height, .01f, 100.f);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.f, 8.f, 0.f));
	model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
	model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(3.0f); 
    glBindVertexArray(VAOs[6]);

	static float t = 0.0f;
	const float speed = 0.5f; 
	const int totalPoints = shooting_star.size() / 6;
	const int trailLength = 20;

    // Draw the line one part at a time
	int startIdx = (int)(t * totalPoints);
	int endIdx = std::min(startIdx + trailLength, totalPoints);
	int count = endIdx - startIdx;

	if (count > 0) {
		glDrawArrays(GL_LINE_STRIP, startIdx, count);
	}

	// Update time parameter
	t += speed * deltaTime; 
	if (t > 1.0f) {
		t = 0.0f;
		// Give the star a new path
		update_shooting_star_path();
	}
}

void draw_beam(unsigned int program) {
	glBindVertexArray(VAOs[4]);
	glm::mat4 modelCylinder = glm::mat4(1.0f);
	if (!is_clicked) {
		// Apply transformations
		modelCylinder = glm::scale(modelCylinder, glm::vec3(1.f, 1.f, 1.f));
		modelCylinder = glm::translate(modelCylinder, glm::vec3(0.f, 1.f, 0.f));
		// Rotate in the Y to be upright
		modelCylinder = glm::rotate(modelCylinder, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
	}
	else {
		if (beam_animation) {
			float time_now = glfwGetTime();
			// Time since animation started
			float t = time_now - start_time;
			float duration = 3.f;
			float half = duration / 2.0f;
			float clamped_t = glm::min(t, duration);

			glm::vec3 scale = glm::vec3(1.f);

			if (clamped_t < half) {
				// Shrink X and Z
				float progress = clamped_t / half;
				float horizontal_scale = glm::mix(1.0f, 0.01f, progress);
				scale = glm::vec3(horizontal_scale, 1.0f, horizontal_scale);
			}
			else {
				// Shrink Y
				float progress = (clamped_t - half) / half;
				float vertical_scale = glm::mix(1.0f, 0.01f, progress);
				scale = glm::vec3(0.01f, vertical_scale, 0.01f);
			}

			// Apply transforms
			modelCylinder = glm::translate(modelCylinder, glm::vec3(0.f, 1.f, 0.f));
			modelCylinder = glm::rotate(modelCylinder, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
			modelCylinder = glm::scale(modelCylinder, scale);

			if (t >= duration) {
				beam_animation = false;
			}
		}
		else {
			// Scale to be very small
			modelCylinder = glm::scale(modelCylinder, glm::vec3(0.f, 0.f, 0.f));
		}
	}

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelCylinder));

	int num_object_vertices = cylinder.size() / 11;
	// Draw cylinder
	glDrawArrays(GL_TRIANGLES, 0, num_object_vertices);
}

void draw_ufo(unsigned int program) {
	// Bind the VAO
	glBindVertexArray(VAOs[2]);
	glm::mat4 modelUFO = glm::mat4(1.0f);
	// If UFO not clicked
	if (!is_clicked) {
		// Apply transformations to the UFO
		modelUFO = glm::translate(modelUFO, glm::vec3(0.f, 1.5f, 0.f));
		modelUFO = glm::rotate(modelUFO, (float)glfwGetTime() / 20, glm::vec3(0.f, 1.f, 0.f));
		modelUFO = glm::scale(modelUFO, glm::vec3(0.06f, 0.06f, 0.06f));
	}
	else {
		if (ufo_animation) {
			// If UFO clicked, animation
			// Time since animation started
			float current_time = glfwGetTime();
			float t = current_time - start_time;

			// Animation speed
			float duration = 5.0f;

			// Clamp t to 0 and duration
			float clamped_t = glm::min(t, duration);

			float y = glm::mix(1.5f, 10.0f, clamped_t / duration);
			float z = glm::mix(0.0f, -10.0f, clamped_t / duration);

			// Scale shrinks from normal size to very small
			float scale = glm::mix(0.06f, 0.005f, clamped_t / duration);

			modelUFO = glm::translate(modelUFO, glm::vec3(0.f, y, z));
			modelUFO = glm::rotate(modelUFO, current_time / 20, glm::vec3(0.f, 1.f, 0.f));
			modelUFO = glm::scale(modelUFO, glm::vec3(scale));

			// Optionally stop animation after duration
			if (t >= duration) {
				ufo_animation = false;
			}
		}
		else {
			// Keep UFO very small
			modelUFO = glm::scale(modelUFO, glm::vec3(0.f, 0.f, 0.f));
		}
	}
	
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelUFO));


	int num_object_vertices = ship_array.size() / 11;
	// Draw the UFO
	glDrawArrays(GL_TRIANGLES, 0, num_object_vertices);

}

void draw_dunes(unsigned int program) {
	// Flat Plane
	glBindVertexArray(VAOs[3]);
	glm::mat4 modelFlatPlane = glm::mat4(1.0f);
	// Move right and forwards and down 
	modelFlatPlane = glm::translate(modelFlatPlane, glm::vec3(6.f, -0.1f, 1.f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelFlatPlane));
	glDrawArrays(GL_TRIANGLES, 0, desert_dunes.size() / 11);
}

void draw_pyramid(unsigned int program) {
	// Pyramid
	glBindVertexArray(VAOs[0]);
	glm::mat4 modelTriangle = glm::mat4(1.0f);
	modelTriangle = glm::scale(modelTriangle, glm::vec3(1.f, 1.f, 1.f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelTriangle));
	int num_vertices = sizeof(double_pyramid_vertices) / (11 * sizeof(float));
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);
}

void draw_jet(unsigned int program) {
	// Bind the VAO
	glBindVertexArray(VAOs[5]);

	// Use time as an angle for the circular motion.
	float angle = glfwGetTime() / 2;
	// Radius around the central point that the plane will travel
	float radius = 45.0f;
	// Position on the path of the circle
	float x = radius * cos(angle);
	float z = radius * sin(angle);
	float y = 30.0f; 

	// Apply transformations to the Jet
	glm::mat4 modelJet = glm::mat4(1.0f);
	modelJet = glm::scale(modelJet, glm::vec3(0.08f, 0.08f, 0.08f));
	// Move to the side of the UFO, initial position
	modelJet = glm::translate(modelJet, glm::vec3(x, y, z));
	
	// Rotate to be perpendicular to tangent of path.
	modelJet = glm::rotate(modelJet, -angle, glm::vec3(0.f, 1.f, 0.f));
	// Bank the plane towards the center of the target.
	modelJet = glm::rotate(modelJet, glm::radians(25.0f), glm::vec3(0.f, 0.f, 1.f));

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelJet));


	int num_object_vertices = jet_array.size() / 11;
	// Draw the Plane
	glDrawArrays(GL_TRIANGLES, 0, num_object_vertices);
}

void draw_skybox(unsigned int program) {
	glUseProgram(program);

	// Save current depth function state
	GLint oldDepthFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunc);

	// Change depth function so depth test passes
	glDepthFunc(GL_LEQUAL);
	// Dont write to depth buffer
	glDepthMask(GL_FALSE);

	// Activate texture unit and bind texture
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_tex);

	// Set the uniform to 10
	glUniform1i(glGetUniformLocation(program, "skybox"), 10);

	// Bind skybox VAO
	glBindVertexArray(VAOs[7]);

	// Set view and projection matrices
	glm::mat4 view = glm::mat4(glm::mat3(GetViewMatrix(*activeCamera)));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)width / (float)height, .01f, 100.f);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// Draw the cube
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Restore original state
	glDepthMask(GL_TRUE);
	glDepthFunc(oldDepthFunc);

	// Unbind
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void generate_depth_map(unsigned int shadowShaderProgram, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix) {
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
	draw_dunes(shadowShaderProgram);
	draw_jet(shadowShaderProgram);

	// Unbind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void render_with_shadow(unsigned int renderShaderProgram, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix) {
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
	glUniform3f(glGetUniformLocation(renderShaderProgram, "camPos"), activeCamera->Position.x, activeCamera->Position.y, activeCamera->Position.z);
	// Directional Lighting
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightColour"), 1.f, 0.98f, 0.7f);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	// Spot lighting

	float t = glfwGetTime(); 
	// Rotate each spolight with the UFO
	for (int i = 0; i < NUM_SPOTLIGHTS; ++i) {
		// Space around 360 degrees / 2pi
		float angle = t / 20.0f + i * glm::two_pi<float>() / NUM_SPOTLIGHTS;  
		glm::vec3 dir = glm::normalize(glm::vec3(cos(-angle), -0.6f, sin(-angle)));  
		spotLightDirections[i] = dir;
	}

	// Give false directions to remove spotlights when ship dissapears
	if (!is_clicked) {
		glUniform3fv(glGetUniformLocation(renderShaderProgram, "spotLightDirections"), NUM_SPOTLIGHTS, glm::value_ptr(spotLightDirections[0]));
	}
	else {
		std::vector<glm::vec3> dummyDirections(NUM_SPOTLIGHTS, glm::vec3(0.0f));
		glUniform3fv(glGetUniformLocation(renderShaderProgram, "spotLightDirections"), NUM_SPOTLIGHTS, glm::value_ptr(dummyDirections[0]));
	}

	
	glUniform3f(glGetUniformLocation(renderShaderProgram, "spotLightPos"), spotLightPos.x, spotLightPos.y, spotLightPos.z);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "spotLightColour"), 0.f, 1.f, 0.f);

	glUniform1f(glGetUniformLocation(renderShaderProgram, "spotLightInnerCutoff"), 45.f);
	glUniform1f(glGetUniformLocation(renderShaderProgram, "spotLightOuterCutoff"), 45.f);

	view = glm::mat4(1.f);
	view = glm::lookAt(activeCamera->Position, activeCamera->Position + activeCamera->Front, activeCamera->Up);
	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	projection = glm::mat4(1.f);
	projection = glm::perspective(glm::radians(fov), (float)width / (float)height, .01f, 100.f);
	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	draw_pyramid(renderShaderProgram);

	// ---- Flat Plane ----
	// Activate sand texture in texture unit 8
	glActiveTexture(GL_TEXTURE8);
	glUniform1i(glGetUniformLocation(renderShaderProgram, "tex0"), 8);
	draw_dunes(renderShaderProgram);

	// --- Cylinder ---
	// Deactivate textures
	glUniform1i(glGetUniformLocation(renderShaderProgram, "uses_texture"), false);
	draw_beam(renderShaderProgram);
	// Re-activate textures
	glUniform1i(glGetUniformLocation(renderShaderProgram, "uses_texture"), true);


	// ---- Jet Plane ----
	// Load texture
	glActiveTexture(GL_TEXTURE9);
	glUniform1i(glGetUniformLocation(renderShaderProgram, "tex0"), 9);

	draw_jet(renderShaderProgram);


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
	glfwSetKeyCallback(window, key_callback);
	// Set callback for window resize
	glfwSetWindowSizeCallback(window, resize_callback);
	// Tracks mouse cursor
	glfwSetCursorPosCallback(window, mouse_move_callback);
	// Callback for mouse buttons
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	// Make cursor hidden
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
	GLuint star_shader = CompileShader("star.vert", "star.frag");
	GLuint skybox_shader = CompileShader("skybox.vert", "skybox.frag");
	
	// Initialise the cameras
	initialise_cameras();

	// Create VAO and VBOs
	initialise_buffers();

	
	// Sand using MipMaps
	const char* sand_files[11] = {
		"sand_mipmap/sand_1024x1024.bmp",
		"sand_mipmap/sand_512x512.bmp",
		"sand_mipmap/sand_256x256.bmp",
		"sand_mipmap/sand_128x128.bmp",
		"sand_mipmap/sand_64x64.bmp",
		"sand_mipmap/sand_32x32.bmp",
		"sand_mipmap/sand_16x16.bmp",
		"sand_mipmap/sand_8x8.bmp",
		"sand_mipmap/sand_4x4.bmp",
		"sand_mipmap/sand_2x2.bmp",
		"sand_mipmap/sand_1x1.bmp"
	};
	// Faces for each side of the cubemap
	vector<std::string> faces
	{
		"skybox/right.png",
		"skybox/left.png",
		"skybox/bottom.png",
		"skybox/top.png",
		"skybox/front.png",
		"skybox/back.png"
	};
	// Set up the cubemap texture
	skybox_tex = setup_cubemap(faces);

	sand_tex = setup_mipmaps(sand_files, 11);
	// Texture to load.
	brick_tex = setup_texture("bricks.jpg");
	ship_tex = setup_texture("objs/ufo/ufo_diffuse.png");
	ship_glow = setup_texture("objs/ufo/ufo_diffuse_glow.png");
	ship_normal = setup_texture("objs/ufo/ufo_normal.png");
	ship_specular = setup_texture("objs/ufo/ufo_spec.png");
	ship_bump = setup_texture("objs/ufo/Map__7_Normal_Bump.tga");
	jet_tex = setup_texture("objs/jet/Paint_tex.jpg");
	// Enable blending for transparency
	glEnable(GL_BLEND);
	// Specify blend function 
	// Final colour is combination of source fragment scaled by alpha and destination fragment scaled by 1 - alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Texture Unit 0 is used for Shadow Mapping.
	// Texture Unit 1 is used by the Texture class.

	// Bind to texture units
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

	// Texture unit 8 - Sand
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, sand_tex);

	// Texture unit 9 - Jet Texture
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, jet_tex);

	//Texture unit 10 - Cubemap
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_tex);

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

		// Must be drawn first
		draw_skybox(skybox_shader);

		generate_depth_map(shadow_shader, shadow, projectedLightSpaceMatrix);
		// Render the pyramid with a shadow map
		render_with_shadow(lighting_program, shadow, projectedLightSpaceMatrix);

		draw_star(star_shader);

		

		// First person camera
		if (current_camera == 1) {
			// Update the frame count
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			process_input(window, *activeCamera, deltaTime);
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