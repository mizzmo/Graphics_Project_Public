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


float shadowOnFragment(vec4 FragPosProjectedLightSpace) {
    // Perspective division
    vec3 projCoords = FragPosProjectedLightSpace.xyz / FragPosProjectedLightSpace.w;
    
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Check if fragment is outside the far plane of light's frustum
    if(projCoords.z > 1.0)
        return 0.0;
    
    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    
    // Get current depth value
    float currentDepth = projCoords.z;
    
    // Calculate bias based on depth texture resolution and slope
    vec3 normal = normalize(nor);
    vec3 lightDir = normalize(-lightDirection);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    // Perform Percentage Closer Filtering
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
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
