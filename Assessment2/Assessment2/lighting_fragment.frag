#version 450 core

layout (location = 0) out vec4 fColour;

in vec4 colour;
in vec3 nor;
in vec3 FragPosWorldSpace;
in vec4 FragPosProjectedLightSpace;

in vec3 tangent;
in vec3 bitangent;

uniform sampler2D shadowMap;


uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 camPos;

// Texture 
in vec2 texCoords;
uniform sampler2D tex0;
uniform bool uses_texture;

// Advanced Texture
uniform bool uses_specular;
uniform bool uses_glow;
uniform bool uses_normal;

uniform float bump_scale; 

uniform sampler2D glow_map;
uniform sampler2D normal_map;
uniform sampler2D specular_map;
uniform sampler2D bump_map;



vec3 calculateNormalFromMap() {
    if (!uses_normal) {
        return normalize(nor);
    }
    
    // Sample the normal map 
    vec3 normalColor = texture(normal_map, texCoords).rgb;
    
    // Transform from [0,1] range to [-1,1] range
    vec3 normalTangentSpace = normalize(normalColor * 2.0 - 1.0);
    
    // Transform from tangent space to world space
    vec3 N = normalize(nor);
    vec3 T = normalize(tangent);

    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(bitangent);
    
    mat3 TBN = mat3(T, B, N);
    
    // Transform normal from tangent space to world space
    return normalize(TBN * normalTangentSpace);
}

float calculateSpecular(vec3 normal, vec3 viewDir, vec3 lightDir) {
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    
    if (uses_specular) {
        // Sample from specular map
        float specularStrength = texture(specular_map, texCoords).r;
        return spec * specularStrength;
    }
    
    // Default specular intensity if no specular map is used
    return spec * 0.5;
}

vec3 calculateNormalFromBumpMap() {

    // Sample neighboring texels for height values
    float step = 1.0/2048.0; 
    
    float heightCenter = texture(bump_map, texCoords).r;
    float heightRight = texture(bump_map, texCoords + vec2(step, 0.0)).r;
    float heightUp = texture(bump_map, texCoords + vec2(0.0, step)).r;
    
    // Calculate gradients
    vec3 normal = normalize(nor);
    vec3 tangent = normalize(tangent);
    vec3 bitangent = normalize(bitangent);
    
    // Calculate normal from the gradient
    vec3 bumpNormal = normalize(
        tangent * (heightRight - heightCenter) * bump_scale +
        bitangent * (heightUp - heightCenter) * bump_scale +
        normal
    );
    
    return bumpNormal;
}


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

    // Advanced Textures for OBJ Files
     if (uses_normal) {
        Nnor = calculateNormalFromMap();
    } else if (bump_scale > 0.0) { 
        Nnor = calculateNormalFromBumpMap();
    } else {
        Nnor = normalize(nor);
    }

    vec3 Nto_light = normalize(-lightDirection); 

    float iDiff = max(dot(Nnor, Nto_light), 0.f);

    vec3 Nfrom_light = normalize(lightPos - FragPosWorldSpace);
    vec3 nRefLight = reflect(-Nto_light, Nnor);  

    vec3 camDirection = camPos - FragPosWorldSpace;
    vec3 NcamDirection = normalize(camDirection);

    float iSpec = pow(max(dot(NcamDirection, nRefLight), 0.f), 128.f);

    float ambient = 0.1f; 

    float rawShadow = shadowOnFragment(FragPosProjectedLightSpace);

   float transparency = colour.a;

    float shadow = rawShadow * transparency;
    float phong = ambient + (1.0 - shadow) * (iDiff + iSpec);

    return phong;
}




void main()
{


    // Lighting Colour
    float phong = CalculateDirectionalIllumination();

    // Check if the shape uses texture
    if(uses_texture){
        // Texture colour
        vec4 texColor = texture(tex0, texCoords);

        fColour = vec4(phong * texColor.rgb * lightColour, texColor.a);

        // Check if the shape uses glow map
        if(uses_glow == true){
            vec4 glow = texture(glow_map, texCoords);
            //Apply the glow map
            fColour = clamp(texColor + glow * 0.1, 0.0, 1.0);
        }
    }
    else{
        // Use object colour
        fColour = vec4(phong * colour.rgb * lightColour, colour.a);
    }
    
}
