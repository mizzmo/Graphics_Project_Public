#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec3 vCol;

out vec3 col;

void main(){
	gl_Position = vec4(1.f, 1.f, 0.f, 1.f);
	col = vCol;
}