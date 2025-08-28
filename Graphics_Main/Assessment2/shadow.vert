#version 450 core

layout(location = 0) in vec3 vPos;

uniform mat4 projectedLightSpaceMatrix;
uniform mat4 model;

void main(){
	gl_Position = projectedLightSpaceMatrix * model * vec4(vPos, 1.0);
}