
#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

// Section 3: Specify Vertices.
GLfloat vertices[] =
{
	//pos				
	0.f,  0.5f,
	-0.5f, -0.5f,
	0.5f,  -0.5f
};



#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];


int main()
{	
	// Section 1: Make a Window.
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(640, 480, "A Triangle", NULL, NULL);
	glfwMakeContextCurrent(window);

	// Section 2: Initialise OpenGL.
	gl3wInit();

	


	// Section 4: Compile Shaders.
	GLuint program = 0;

	// Section 5: Create buffers, Send to GPU.
	glCreateBuffers(NUM_BUFFERS, Buffers);
	glNamedBufferStorage(Buffers[0], sizeof(vertices), vertices, 0);
	glGenVertexArrays(NUM_VAOS, VAOs);
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 * sizeof(float)), (void*)0);
	glEnableVertexAttribArray(0);

	// Section 6: Render Loop.
	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Section 7: Deinitialisation.
	glfwDestroyWindow(window);
	glfwTerminate();
}