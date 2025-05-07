#version 450 core

layout (location = 0) out vec4 fColour;

// Basic Shader
in vec4 colour;
in vec3 nor;
in vec3 FragPosWorldSpace;
in vec4 FragPosProjectedLightSpace;

uniform float shininess;

in mat3 TBN;


// Shadows
uniform sampler2D shadowMap;

// Directional Lighting
uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 camPos;

// Spot Lighting
#define NUM_SPOTS 4
uniform vec3 spotLightDirections[NUM_SPOTS];
uniform vec3 spotLightColour;
uniform vec3 spotLightPos;
uniform float spotLightInnerCutoff; 
uniform float spotLightOuterCutoff; 

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


// ---- Normal Map ----
vec3 calculateNormalFromMap() {
    if (!uses_normal) {
        return normalize(nor);
    }
    // Sample the normal map 
    vec3 normalColor = texture(normal_map, texCoords).rgb;
    
    // Transform from [0,1] range to [-1,1] range
    vec3 normalTangentSpace = normalize(normalColor * 2.0 - 1.0);
    
    return normalize(TBN * normalTangentSpace);
}


// ---- Specular Map ----
float calculateSpecular(vec3 normal, vec3 viewDir, vec3 lightDir) {
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    if (uses_specular) {
        // Sample from specular map
        float specularStrength = texture(specular_map, texCoords).r;
        return spec * specularStrength;
    }
    
    // Default specular intensity if no specular map is used
    return spec * 0.5;
}

// ---- Shadows ----
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


// ---- Blinn-Phong Directional Illumination  ----
vec3 CalculateDirectionalIllumination() {
    vec3 Nnor = normalize(nor);

    // Normal and Bump Maps
    if (uses_normal) {
        Nnor = calculateNormalFromMap();
    }

    // Directional light vector
    vec3 NLightDirection = normalize(-lightDirection); 
    // View direction
    vec3 NViewDirection = normalize(camPos - FragPosWorldSpace); 
    // Halfway vector for Blinn
    vec3 NHalfWayVec = normalize(NLightDirection + NViewDirection); 

    float iDiff = max(dot(Nnor, NLightDirection), 0.0);

    // Specular Component
    float iSpec;
    if (uses_specular) {
        // Use specular map
        iSpec = calculateSpecular(Nnor, NViewDirection, NLightDirection);
    } else {
        iSpec = pow(max(dot(Nnor, NHalfWayVec), 0.0), shininess);
    }

    // Ambient Lighting
    float ambient = 0.1;
    // Shadow before applying alpha
    float rawShadow = shadowOnFragment(FragPosProjectedLightSpace);
    // Apply colour alpha for shadow intensity
    float shadow = rawShadow * colour.a;

    // Calculate final colour
    float diffuseIntensity = (1.0 - shadow) * iDiff;
    float specularIntensity = (1.0 - shadow) * iSpec;
    
    return lightColour * (ambient + diffuseIntensity + specularIntensity);
}

// ---- Spot Illumination ----
vec3 CalculateSpotIllumination() {
    vec3 Nnor = normalize(nor);
    
    // Normal or Bump mapping
    if (uses_normal) {
        Nnor = calculateNormalFromMap();
    }

    vec3 totalLight = vec3(0.0);
    vec3 viewDir = normalize(camPos - FragPosWorldSpace);

    // Loop through each spotlight
    for (int i = 0; i < NUM_SPOTS; ++i) {
        // Vector from fragment to spotlight position
        vec3 lightDir = normalize(spotLightPos - FragPosWorldSpace);

        // Direction the spotlight is pointing
        vec3 spotDir = normalize(-spotLightDirections[i]);

        // Angle between light direction and spotlight's central direction
        float cosTheta = dot(spotDir, lightDir);

        // Spotlight inner and outer cutoffs for shape
        float innerCutoff = cos(radians(12.5));
        float outerCutoff = cos(radians(17.5));

        float intensity = 0.0;

        // Determine spotlight intensity based on cone angle
        if (cosTheta > outerCutoff) {
            // Smooth interpolation between outer and inner cutoff
            if (cosTheta < innerCutoff) {
                intensity = smoothstep(outerCutoff, innerCutoff, cosTheta);
            } else {
                intensity = 1.0;
            }

            // --- Blinn-Phong Lighting ---

            // Diffuse term 
            float iDiff = max(dot(Nnor, lightDir), 0.0);

            // Specular term using specular mapping function
            float iSpec;
            if (uses_specular) {
                iSpec = calculateSpecular(Nnor, viewDir, lightDir);
            } else {
                // Halfway vector between light and view directions
                vec3 halfwayDir = normalize(lightDir + viewDir);
                iSpec = pow(max(dot(Nnor, halfwayDir), 0.0), shininess);
            }

            // Attenuation based on distance to the light
            float distance = length(spotLightPos - FragPosWorldSpace);
            float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

            // Lighting components
            float ambient = 0.1;
            float diffuse = iDiff * intensity;
            float specular = iSpec * intensity;

            // Accumulate light contribution from this spotlight
            totalLight += spotLightColour * (ambient + (diffuse + specular) * attenuation);
        }
    }

    return totalLight;
}

// Apply glow map to any color
vec4 applyGlowMap(vec4 baseColor) {
    if (uses_glow) {
        vec4 glowColor = texture(glow_map, texCoords);
        // Intensity of glow
        float intensity = 0.4f;
        return clamp(baseColor + glowColor * intensity, 0.0, 1.0);
    }
    return baseColor;
}

void main()
{

    
    // Calculate light contributions
    vec3 dirLightColor = CalculateDirectionalIllumination();
    vec3 spotLightColor = CalculateSpotIllumination(); 
    
    // Combine both light sources
    vec3 finalLightColor = dirLightColor + spotLightColor;
    
    // Apply to base color
    if (uses_texture) {
        vec4 texColor = texture(tex0, texCoords);
        // Apply lighting to texture color
        vec4 litColor = vec4(finalLightColor * texColor.rgb, texColor.a);
        // Apply glow effect
        fColour = applyGlowMap(litColor);
    } else {
        // For untextured objects
        vec4 litColor = vec4(finalLightColor * colour.rgb, colour.a);
        // Apply glow effect
        fColour = applyGlowMap(litColor);
    }
}