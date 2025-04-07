#version 450 core

// Position Coordinates
layout(location = 0) in vec3 vPos;
// Colour
layout(location = 1) in vec3 vColor;
// Texture Coordinates
layout (location = 2) in vec2 vTexture;

out vec3 colour;
out vec2 texCoords;

// Declare uniforms for 3 space matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	// Set the position of the vertex in 3D space.
	gl_Position = projection * view * model * vec4(vPos, 1.0);
	// Set the colour output.
	colour = vColor;
	texCoords = vTexture;
}