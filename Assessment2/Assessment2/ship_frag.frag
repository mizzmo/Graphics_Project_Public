#version 450 core

layout (location = 0) out vec4 fColour;

in vec2 texCoords; 

uniform sampler2D ship_tex;
uniform sampler2D glow_map;

void main() {
	vec4 texColor = texture(ship_tex, texCoords);
    vec4 glow = texture(glow_map, texCoords);
    
    // Red colour to indicate error with textures
    if(texColor.a < 0.1) { 
        fColour = vec4(1.0, 0.0, 0.0, 1.0);  
    } else {

        // Add the glow map. Clamp to avoid over brightening
        fColour = clamp(texColor + glow * 0.5, 0.0, 1.0);
    }
}
