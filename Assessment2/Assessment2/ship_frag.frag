#version 450 core

layout (location = 0) out vec4 fColour;

in vec2 TexCoords; 

uniform sampler2D ship_tex;

void main() {
	fColour = texture(ship_tex, TexCoords); 
	// Set to red so i can see it
	fColour = vec4(1.f, 0.f, 0.f, 1.0); 
}
