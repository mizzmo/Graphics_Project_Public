#version 450 core

layout (location = 0) out vec4 fColour;

in vec3 colour;
in vec3 nor;
in vec3 FragPosWorldSpace;
in vec4 FragPosProjectedLightSpace;

uniform sampler2D shadowMap;


uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 camPos;

// Texture 
in vec2 texCoords;
uniform sampler2D tex0;


float shadowOnFragment(vec4 FragPosProjectedLightSpace){
    // Convert the projected fragment position to normalized device coordinates (NDC)
    vec3 ndc = FragPosProjectedLightSpace.xyz / FragPosProjectedLightSpace.w;

    // Scale and bias the NDC to get screen-space coordinates
    vec3 ss = (ndc + 1) * 0.5;

    // Get the depth of the fragment from the screen-space coordinates
    float fragDepth = ss.z;

    // Initialize the shadow value to 0 (no shadow)
    float shadow = 0.f;

    // Sample the depth from the shadow map at the screen-space coordinates
    float litDepth = texture(shadowMap, ss.xy).r;

    vec3 Nnor = normalize(nor);
    vec3 Ntolight = normalize(-lightDirection);
    float bias = max(0.05 * (1.0 - dot(Nnor, Ntolight)) ,0.005);

    // Compare the fragment depth to the depth from the shadow map to determine if the fragment is in shadow
    shadow = fragDepth > (litDepth+bias) ? 1.0 : 0.0;

    if(fragDepth > 1){
        shadow = 0.f;
    }

    // Return the shadow value
    return shadow;
}

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

    float shadow = shadowOnFragment(FragPosProjectedLightSpace);

    float phong = ambient + ((1.f - shadow) * (iDiff + iSpec));

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
