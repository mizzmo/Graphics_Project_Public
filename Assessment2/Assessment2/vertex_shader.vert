#version 450 core

// Position Coordinates
layout(location = 0) in vec3 vPos;
// Colour
layout(location = 1) in vec3 vColor;
// Texture Coordinates
layout (location = 2) in vec2 vTexture;

out vec3 colour;
out vec2 texCoords;

void main()
{
	// Set the position of the vertex
	gl_Position = vec4(vPos, 1.0);
	// Set the colour output.
	colour = vColor;
	texCoords = vTexture;
}