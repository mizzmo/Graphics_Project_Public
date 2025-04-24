#version 450 core

layout (location = 0) out vec4 fColour;

in vec3 colour;
in vec3 nor;
in vec3 FragPosWorldSpace;


uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 camPos;

// Texture 
in vec2 texCoords;
uniform sampler2D tex0;

float CalculateDirectionalIllumination(){
    vec3 Nnor = normalize(nor);
    vec3 Nto_light = normalize(-lightDirection); 

    float iDiff = max(dot(Nnor, Nto_light), 0.f);

    vec3 Nfrom_light = normalize(lightPos - FragPosWorldSpace);
    vec3 nRefLight = reflect(-Nto_light, Nnor);  

    vec3 camDirection = camPos - FragPosWorldSpace;
    vec3 NcamDirection = normalize(camDirection);

    float iSpec = pow(max(dot(NcamDirection, nRefLight), 0.f), 128.f);

    float ambient = 0.1f; 
    float phong = ambient + iDiff + iSpec;

    return phong;
}




void main()
{
    // Lighting Colour
    float phong = CalculateDirectionalIllumination();

    // Texture colour
    vec4 texColor = texture(tex0, texCoords);

    fColour = vec4(phong * texColor.rgb * lightColour, texColor.a);
}
