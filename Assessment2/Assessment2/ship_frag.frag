#version 450 core

layout (location = 0) out vec4 fColour;

in vec2 TexCoords; 

uniform sampler2D ship_tex;

void main() {
	fColour = texture(ship_tex, TexCoords);  
}
