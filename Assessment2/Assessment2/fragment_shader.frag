#version 450 core

layout (location = 0) out vec4 fColour;

in vec3 colour;
in vec2 texCoords;

uniform sampler2D tex0; // Brick texture for the object
uniform sampler2D tex1; // Grass texture for the plane

void main() {
    if (gl_FragCoord.z == 1.0) { 
        fColour = texture(tex1, texCoords);  // Grass texture
    } else {
        fColour = texture(tex0, texCoords);  // Brick texture
    }
}
