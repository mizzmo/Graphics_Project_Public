#version 450 core

layout (location = 0) out vec4 fColour;

in vec3 colour;
in vec2 texCoords;

uniform sampler2D tex0;

void main(){
	// Set colour using input colour.
	//fColour = vec4(colour, 1.f);

	// Set colour using Texture. 
	fColour = texture(tex0, texCoords);
}
