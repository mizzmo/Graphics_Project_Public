#pragma once
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if (type == GL_DEBUG_TYPE_ERROR) {
		fprintf(stderr, "Debug Callback: ERROR - %s\n", message);
	}
}