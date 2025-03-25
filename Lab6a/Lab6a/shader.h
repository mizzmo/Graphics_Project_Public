#pragma once
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "file.h"


GLuint CompileShader(const char* vsFilename, const char* fsFilename)
{
	int success;
	char infoLog[512];

	// Compile Vertex Shader.
	// Create a shader and return handle for referencing it.
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Reads a shader from a file.
	char* vertexShaderSource = read_file(vsFilename);
	// Sets the shader source as specified in the string to the shader.
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// Compiles the shader.
	glCompileShader(vertexShader);

	// Checks for compilation errors in shader, store result in success.
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		// Queries info log of shader, specifies an array of characters used to return the information log.
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		fprintf(stderr, "Vertex Shader Compilation Fail - %s\n", infoLog);
	}

	// Compile Fragment Shader.
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char* fragmentShaderSource = read_file(fsFilename);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		fprintf(stderr, "Fragment Shader Compilation Fail -%s\n", infoLog);
	}

	// Link Program.
	// Create shader program
	unsigned int program = glCreateProgram();
	// Attach shaders to program, turns attached shaders into executable code.
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	// Link the program
	glLinkProgram(program);

	// Checks for errors in program, store in success.
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		// Queries log of program and return information as list of chars.
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		fprintf(stderr, "Shader Program Link Fail - %s\n", infoLog);
	}

	free(fragmentShaderSource);
	free(vertexShaderSource);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}
