#version 450 core

layout (location = 0) out vec4 fColour;

in vec3 colour;

void main() {
    fColour = vec4(colour, 1.0); 
}
