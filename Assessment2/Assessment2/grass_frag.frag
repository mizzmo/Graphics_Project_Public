#version 450 core

layout (location = 0) out vec4 fColour;

in vec3 colour;
in vec2 texCoords;

uniform sampler2D tex; 

void main() {
	fColour = texture(tex, texCoords); 
}
