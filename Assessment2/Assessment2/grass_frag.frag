#version 450 core

layout (location = 0) out vec4 fColour;

in vec3 colour;
in vec2 texCoords;

uniform sampler2D tex1; // Grass texture for the plane

void main() {
	fColour = texture(tex1, texCoords);  // Brick texture
}
