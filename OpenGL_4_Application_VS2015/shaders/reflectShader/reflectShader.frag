#version 400 core

in vec3 normal;
in vec4 fragPosEye;

out vec4 fragColor;

uniform mat3 normalMatrix;
uniform samplerCube skybox;

void main()
{

	vec3 viewDirection = -fragPosEye.xyz;
	vec3 viewDirectionN = normalize(viewDirection);
	
	//vec3 normalN = normalize(normal);
	//vec3 reflection = reflect(viewDirectionN, normalN);
	
	vec3 fragPosEyeN = normalize(normalMatrix * normal);
	vec3 reflection = reflect(viewDirectionN, fragPosEyeN);
	
	vec3 colorFromSkybox = vec3(texture(skybox, reflection));
	
	fragColor = vec4(colorFromSkybox, 1.0f);
}