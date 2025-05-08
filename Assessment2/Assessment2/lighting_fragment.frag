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

// PBR Textures
uniform bool uses_pbr;
// Base color texture
uniform sampler2D albedoMap;     
// Additional Textures for details
uniform sampler2D metallicMap; 
uniform sampler2D roughnessMap;  
uniform sampler2D aoMap;        

// Environment Lighting
// Colour
uniform vec3 ambientLight; 
// How bright
uniform float environmentIntensity;

// Value of Pi
const float PI = 3.14159265359;

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
vec3 CalculateDirectionalIllumination(vec3 Nnor, vec3 viewDir) {

    // Directional light vector
    vec3 NLightDirection = normalize(-lightDirection); 
    // Halfway vector for Blinn
    vec3 NHalfWayVec = normalize(NLightDirection + viewDir); 

    float iDiff = max(dot(Nnor, NLightDirection), 0.0);

    // Specular Component
    float iSpec;
    if (uses_specular) {
        // Use specular map
        iSpec = calculateSpecular(Nnor, viewDir, NLightDirection);
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
vec3 CalculateSpotIllumination(vec3 Nnor, vec3 viewDir) {
   
    vec3 totalLight = vec3(0.0);
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


// ---- PBR Lighting Stuff ----
// Normal Distribution
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return a2 / max(denom, 0.0000001);
}

// Geometry Function
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    return NdotV / (NdotV * (1.0 - k) + k);
}

// Combined Geometry term
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

// Fresnel Equation
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


vec3 CalculatePBR(vec3 N, vec3 V, vec3 albedoValue, float metallicValue, float roughnessValue, float aoValue)
{
    // Surface reflection at zero incidence
    vec3 F0 = vec3(0.04, 0.04, 0.04);
    F0 = mix(F0, albedoValue, metallicValue); 
    
    // Outgoing radiance
    vec3 Lo = vec3(0.0);
    
    // Increase prevelance of small details
    aoValue = pow(aoValue, 2);
    
    // Calculate directional light contribution
    // Light direction
    vec3 L = normalize(-lightDirection);
    // Half vector between view and light directions
    vec3 H = normalize(V + L);
        
    // Calculate light radiance
    vec3 radiance = lightColour;
        
    // Calculate shadows
    float shadow = shadowOnFragment(FragPosProjectedLightSpace) * 0.5;
        
    // Calculate BRDF
    float NDF = DistributionGGX(N, H, roughnessValue);
    float G = GeometrySmith(N, V, L, roughnessValue);
    vec3 F = FresnelSchlickRoughness(max(dot(H, V), 0.0), F0, roughnessValue);
        
    // Specular component
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
        
    // Calculate diffuse and specular contribution
    vec3 kS = F; 
    vec3 kD = vec3(1.0) - kS; 
    kD *= 1.0 - metallicValue;
        
    // Combine components: diffuse + specular
    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * albedoValue / PI + specular) * radiance * NdotL * (1.0 - shadow);
    
    
    // Calculate spot lights
    for (int i = 0; i < NUM_SPOTS; ++i) {
        // Vector from fragment to spotlight position
        vec3 L = normalize(spotLightPos - FragPosWorldSpace);
        
        // Direction the spotlight is pointing
        vec3 spotDir = normalize(-spotLightDirections[i]);
        
        // Angle between light direction and spotlight's central direction
        float cosTheta = dot(spotDir, L);
        
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
            
            // Half vector
            vec3 H = normalize(V + L);
            
            // Calculate light attenuation 
            float distance = length(spotLightPos - FragPosWorldSpace);
            float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
            vec3 radiance = spotLightColour * attenuation * intensity;
            
            // Calculate BRDF
            float NDF = DistributionGGX(N, H, roughnessValue);
            float G = GeometrySmith(N, V, L, roughnessValue);
            vec3 F = FresnelSchlickRoughness(max(dot(H, V), 0.0), F0, roughnessValue);
            
            // Calculate specular component
            vec3 numerator = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
            vec3 specular = numerator / denominator;
            
            // Calculate diffuse and specular contribution
            vec3 kS = F; 
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallicValue;
            
            // Combine diffuse and specular
            float NdotL = max(dot(N, L), 0.0);
            Lo += (kD * albedoValue / PI + specular) * radiance * NdotL;
        }
    }
    
    // Ambient lighting
    vec3 ambient = vec3(0.1) * albedoValue * aoValue;
    
    // Final color with ambient
    vec3 color = ambient + Lo;
    
    // HDR tonemapping
    float exposure = 1.5;
    vec3 mapped = vec3(1.0) - exp(-color * exposure);
    
    // Adjust brightness based on metallic value
    float brightnessFactor = mix(1.0, 2.0, metallicValue * (1.0 - roughnessValue));
    mapped *= brightnessFactor;
    
    // Gamma correction
    color = pow(mapped, vec3(1.0/2.2));
    
    return color;
}

void main()
{
    // Get correct normal depending on which technique in use
    // We calculate in main to be consistent accross each process
    vec3 N;
    if (uses_normal && uses_parrallax) {
        N = ParallaxMappedNormal();
    } else if (uses_normal) {
        N = calculateNormalFromMap();
    } else {
        N = normalize(nor);
    }
    
    // View direction
    vec3 V = normalize(camPos - FragPosWorldSpace);
    
    // Texture coordinates 
    vec2 currentTexCoords = texCoords;
    if (uses_parrallax) {
        // Transform positions into tangent space
        vec3 TangentViewPos = TBN * camPos;
        vec3 TangentFragPos = TBN * FragPosWorldSpace;
        vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
        
        // Apply parallax mapping to texture coordinates
        currentTexCoords = ParallaxOcclusionMapping(texCoords, viewDir);
        currentTexCoords = clamp(currentTexCoords, 0.0, 1.0);
    }
    
    // Apply UV scaling for texture size 
    vec2 scaledTexCoords = currentTexCoords * uv_scale;
    
    vec4 finalColor;
    
    // PBR
    if (uses_pbr) {
        // Get PBR material properties
        vec3 albedoValue;
        float metallicValue;
        float roughnessValue;
        float aoValue;
    
        // Ensure scaled coordinates are correct for PBR
        vec2 pbrTexCoords = currentTexCoords * uv_scale;
    
        // Sample textures with the proper scaling
        albedoValue = texture(albedoMap, pbrTexCoords).rgb;
        metallicValue = texture(metallicMap, pbrTexCoords).r;
        roughnessValue = texture(roughnessMap, pbrTexCoords).r;
        aoValue = texture(aoMap, pbrTexCoords).r;
        
        // Calculate lighting using PBR
        vec3 pbrColor = CalculatePBR(N, V, albedoValue, metallicValue, roughnessValue, aoValue);

        // Simulate environmental lighting for better reflections
        vec3 ambient = vec3(0.03) * albedoValue * aoValue;

        vec3 reflectDir = reflect(-V, N);
        vec3 envReflection = ambientLight * environmentIntensity;

        float reflectionStrength = (1.0 - roughnessValue) * mix(0.2, 1.0, metallicValue);
        ambient += envReflection * reflectionStrength * albedoValue;
        
        finalColor = vec4(pbrColor, 1.0);
    } else {
        // Use the non-PBR lighting
        vec3 dirLightColor = CalculateDirectionalIllumination(N, V);
        vec3 spotLightColor = CalculateSpotIllumination(N, V); 
        
        // Combine both light sources
        vec3 finalLightColor = dirLightColor + spotLightColor;
        
        // Apply to base color
        if (uses_texture) {
            vec4 texColor = texture(tex0, scaledTexCoords);
            finalColor = vec4(finalLightColor * texColor.rgb, texColor.a);
        } else {
            finalColor = vec4(finalLightColor * colour.rgb, colour.a);
        }
    }
    
    fColour = finalColor;
}
