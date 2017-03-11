#version 400 core

in vec3 normal;
in vec4 fragPosEye;
in vec2 fragTexCoords;
in vec4 lightPointEye;
in vec4 lightPointEye2;
in vec4 fragPosLightSpace;

out vec4 fColor;

//lighting
uniform	vec3 lightColor;
uniform vec3 lightDir;
uniform	mat3 normalMatrix;
uniform int activeDirL;
uniform int activePointL;
uniform mat3 lightDirMatrix;

uniform sampler2D diffuseTexture; //harta difuza
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

uniform float ambientStrength_Point;

vec3 orangeColor = vec3(1.0f, 0.55f, 0.0f); //orange
vec3 yellowColor = vec3(1.0f, 0.84f, 0.0f);

vec4 ambient;
float ambientStrength = 0.5f;
vec4 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

//attenuation
float constant = 1.0f;
float linear = 0.045f;
float quadratic = 0.0075f;
float shadow;


float computeShadow(vec3 lightPassed)
{
	// perform perspective divide 
	// returns the current fragment’s position in the [-1,1] range
			vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
			
			if (normalizedCoords.z > 1.0f)
				return 0.0f;
	// BUT: fragment depth values are in the [0,1] range
	// Transform to [0,1] range
			normalizedCoords = normalizedCoords * 0.5 + 0.5;
	
	// Get closest depth value from light's perspective
			float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	
	// Get depth of current fragment from light's perspective
			float currentDepth = normalizedCoords.z;

	// comparing
			//float bias = 0.005f;
			float bias = max(0.05f * (1.0f - dot(normal, lightPassed)), 0.005f);
			float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

	return shadow;
}

float computeFog()
{
 float fogDensity = 0.04f;
 float fragmentDistance = length(fragPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

vec3 dirLight()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin

	vec3 normalEye = normalize(normalMatrix * normal);	
	
	vec3 lightDirN = normalize(lightDirMatrix * lightDir);


	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	vec3 halfVector = normalize(lightDirN + viewDirN);

	ambient.xyz = ambientStrength * lightColor;

	diffuse.xyz = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;

	
	ambient *= texture(diffuseTexture, fragTexCoords);
	//if(ambient.a < 0.1)
		//discard;
	diffuse *= texture(diffuseTexture, fragTexCoords);
	//if(diffuse.a < 0.1)
		//discard;
	specular *= vec3(texture(specularTexture, fragTexCoords));

	return min((ambient.xyz + (1.0f - shadow)* diffuse.xyz) + (1.0f - shadow) * specular , 1.0f);
}

vec3 pointLight(vec3 lightPointSource, vec3 lightColorLocal)
{
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	

	vec3 normalEye = normalize(normalMatrix * normal);	
	
	vec3 lightPointEyeN = normalize(lightPointSource - fragPosEye.xyz);
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	vec3 halfVector = normalize(lightPointEyeN + viewDirN);

	//compute distance to light
	float dist = length(lightPointEye.xyz - fragPosEye.xyz);
	//compute attenuation
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));


	ambient.xyz = att * ambientStrength_Point * lightColorLocal;

	diffuse.xyz = att * max(dot(normalEye, lightPointEyeN), 0.0f) * lightColorLocal;
	
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular = att * specularStrength * specCoeff * lightColorLocal;

	ambient *= texture(diffuseTexture, fragTexCoords);
	//if(ambient.a < 0.1)
		//discard;
	diffuse *= texture(diffuseTexture, fragTexCoords);
	//if(diffuse.a < 0.1)
		//discard;
	specular *= texture(specularTexture, fragTexCoords);

	return min((ambient.xyz + diffuse.xyz) + specular , 1.0f);
}

void main() 
{
	vec3 color = vec3(0.0f);

	shadow = computeShadow(lightDir);

	if(activeDirL==1)
		color += dirLight();
	if(activePointL==1)
		color += pointLight(lightPointEye.xyz, lightColor);
	
	color += pointLight(lightPointEye2.xyz, yellowColor);
 
		
	//FOG
	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.3f, 0.4f, 0.2f, 1.0f); //darkolivegreen
	
	fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);

}
