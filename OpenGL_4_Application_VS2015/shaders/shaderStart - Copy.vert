#version 400 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 normal;
out vec4 fragPosEye;
out vec2 fragTexCoords;
out vec4 lightPosEye;
out vec4 fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPoint;
uniform mat4 lightSpaceTrMatrix;

void main() 
{
	//compute eye space coordinates
	fragPosEye = view * model * vec4(vPosition, 1.0f);
	lightPosEye = view * model * vec4(lightPoint, 1.0f);
	fragPosLightSpace = lightSpaceTrMatrix * model * vec4(vPosition, 1.0f);
	normal = vNormal;
	fragTexCoords = vTexCoords;
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
}
