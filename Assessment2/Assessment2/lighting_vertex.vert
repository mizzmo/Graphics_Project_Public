#version 450 core

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec3 vColor;
layout (location = 2) in vec2 vTexture;
layout(location = 3) in vec3 vNor;

out vec3 colour;
out vec2 texCoords;
out vec3 nor;
out vec3 FragPosWorldSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main()
{
	gl_Position = projection * view * model * vPos;
	colour = vColor;
	texCoords = vTexture;
	nor = mat3(transpose(inverse(model))) * vNor;
	FragPosWorldSpace = vec3(model * vPos);
}
