#version 450 core

layout (location = 0) out vec4 fColour;

// Basic Shader
in vec4 colour;
in vec3 nor;
in vec3 FragPosWorldSpace;
in vec4 FragPosProjectedLightSpace;

uniform float shininess;

// Tangent Space
in mat3 TBN;

// Texture Scaling
uniform float uv_scale;

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

// Parallax Mapping
uniform sampler2D depth_map;
uniform float height_scale;
uniform bool uses_parrallax;

// Get scaled texture coordinates
vec2 getScaledTexCoords() {
    return texCoords * uv_scale;
}

// ---- Normal Map ----
vec3 calculateNormalFromMap() {
    if (!uses_normal) {
        return normalize(nor);
    }
    // Sample the normal map with scaled coordinates
    vec2 scaledCoords = getScaledTexCoords();
    vec3 normalColor = texture(normal_map, scaledCoords).rgb;
    
    // Transform from [0,1] range to [-1,1] range
    vec3 normalTangentSpace = normalize(normalColor * 2.0 - 1.0);
    
    return normalize(TBN * normalTangentSpace);
}


// ---- Specular Map ----
float calculateSpecular(vec3 normal, vec3 viewDir, vec3 lightDir) {
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    if (uses_specular) {
        // Sample from specular map with scaled coordinates
        vec2 scaledCoords = getScaledTexCoords();
        float specularStrength = texture(specular_map, scaledCoords).r;
        return spec * specularStrength;
    }
    
    // Default specular intensity if no specular map is used
    return spec * 0.5;
}


// --- Parallax Mapping ---
// --- Improved Parallax Occlusion Mapping ---
vec2 ParallaxOcclusionMapping(vec2 texCoords, vec3 viewDirTangent)
{
    // Use scaled texture coordinates as input
    vec2 scaledCoords = texCoords * uv_scale;
    
    // Adjust view direction for height scale (controls parallax strength)
    vec2 P = viewDirTangent.xy * height_scale / viewDirTangent.z;
    
    // Determine number of layers dynamically based on view angle
    // More layers when looking at grazing angles, fewer when looking straight on
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDirTangent)));
    
    // Calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    
    // Current layer depth
    float currentLayerDepth = 0.0;
    
    // Calculate the texture coordinate delta per layer
    vec2 deltaTexCoords = P / numLayers;
    
    // Initialize values
    vec2 currentTexCoords = scaledCoords;
    float currentDepthMapValue = texture(depth_map, currentTexCoords).r;
    
    // Loop until we find where ray intersects heightmap
    while(currentLayerDepth < currentDepthMapValue)
    {
        // Shift to next layer
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(depth_map, currentTexCoords).r;
        currentLayerDepth += layerDepth;
        
        // Safety check to prevent infinite loops
        if(currentLayerDepth > 1.0) break;
    }
    
    // ---- Parallax Occlusion Mapping refinement step ----
    // Get previous texture coordinates
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    
    // Get depth values for linear interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depth_map, prevTexCoords).r - (currentLayerDepth - layerDepth);
    
    // Calculate interpolation weight
    float weight = afterDepth / (afterDepth - beforeDepth);
    
    // Interpolate between the two closest points
    vec2 finalTexCoords = mix(currentTexCoords, prevTexCoords, weight);
    
    // Return coordinates in unscaled space
    return finalTexCoords / uv_scale;
}

// Compute Normal Using Parallax Mapping
vec3 ParallaxMappedNormal()
{   
    if(!uses_parrallax) {
        return normalize(nor);
    }
    
    // Transform positions into tangent space
    vec3 TangentLightPos = TBN * lightPos;
    vec3 TangentViewPos  = TBN * camPos;
    vec3 TangentFragPos  = TBN * FragPosWorldSpace;

    // Compute view direction in tangent space
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    // Offset texture coordinates using parallax mapping
    vec2 parallaxCoords = ParallaxOcclusionMapping(texCoords, viewDir);

    // Clamp coordinates to avoid artifacts at edges
    parallaxCoords = clamp(parallaxCoords, 0.0, 1.0);

    // Apply scaling for normal map lookup
    vec2 scaledParallaxCoords = parallaxCoords * uv_scale;
    
    // Sample normal map using the offset coordinates
    vec3 normalColor = texture(normal_map, scaledParallaxCoords).rgb;
    vec3 normalTangentSpace = normalize(normalColor * 2.0 - 1.0);
    
    // Transform back to world space 
    return normalize(TBN * normalTangentSpace);
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
    if(uses_parrallax){
        Nnor = ParallaxMappedNormal();
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
    if(uses_parrallax){
        Nnor = ParallaxMappedNormal();
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
        // Use scaled coordinates for glow map
        vec2 scaledCoords = getScaledTexCoords();
        vec4 glowColor = texture(glow_map, scaledCoords);
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
        // Scale texture coordinates
        vec2 scaledTexCoords = getScaledTexCoords();
        vec4 texColor = texture(tex0, scaledTexCoords);
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