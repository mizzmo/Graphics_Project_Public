#version 450 core
layout(location = 0) in vec4 vPos;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec2 vTexture;
layout(location = 3) in vec3 vNor;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 bitangent;

out vec4 colour;
out vec2 texCoords;
out vec3 nor;
out mat3 TBN;
out vec3 FragPosWorldSpace;
out vec4 FragPosProjectedLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 projectedLightSpaceMatrix;

void main()
{
    FragPosWorldSpace = vec3(model * vPos);
    gl_Position = projection * view * model * vPos;
    colour = vColor;
    texCoords = vTexture;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    nor = normalize(normalMatrix * vNor);
    vec3 T = normalize(normalMatrix * tangent);
    vec3 B = normalize(normalMatrix * bitangent);
    vec3 N = nor;    
    T = normalize(T - dot(T, N) * N);   
    B = cross(N, T);
    TBN = mat3(T, B, N);
    FragPosProjectedLightSpace = projectedLightSpaceMatrix * model * vPos;
}