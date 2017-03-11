#version 400 core

in vec3 normal;
in vec4 fragPosEye;
in vec2 fragTexCoords;
in vec4 lightPosEye;
in vec4 fragPosLightSpace;

out vec4 fColor;

//lighting
uniform vec3 lightDir;
uniform	vec3 lightColor;
uniform	mat3 normalMatrix;
uniform int activeDirL;
uniform int activePointL;
uniform mat3 lightDirMatrix;

uniform sampler2D diffuseTexture; //harta difuza
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

vec3 baseColor = vec3(1.0f, 0.55f, 0.0f); //orange
vec3 ambient;
float ambientStrength = 0.5f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

//attenuation
float constant = 1.0f;
float linear = 0.045f;
float quadratic = 0.0075f;
float shadowD, shadowP;


float computeShadow(vec3 lightPassed)
{
	// perform perspective divide 
	// returns the current fragment’s position in the [-1,1] range
			vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
			
	// BUT: fragment depth values are in the [0,1] range
	// Transform to [0,1] range
			normalizedCoords = normalizedCoords * 0.5 + 0.5;

			if (normalizedCoords.z > 1.0f)
	return 0.0f;
	

	
	// Get closest depth value from light's perspective
			float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	
	// Get depth of current fragment from light's perspective
			float currentDepth = normalizedCoords.z;

	// comparing
			float bias = max(0.05f * (1.0f - dot(normal, lightPassed)), 0.005f);
			float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

	return shadow;
}

vec3 dirLight()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin

	vec3 normalEye = normalize(normalMatrix * normal);	
	
	vec3 lightDirN = normalize(lightDirMatrix * lightDir);


	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	vec3 halfVector = normalize(lightDirN + viewDirN);

	ambient = ambientStrength * lightColor;

	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;

	
	ambient *= texture(diffuseTexture, fragTexCoords);
	diffuse *= texture(diffuseTexture, fragTexCoords);
	specular *= texture(specularTexture, fragTexCoords);

	return min((ambient + (1.0f - shadowD)* diffuse) + (1.0f - shadowD) * specular , 1.0f);
}

vec3 pointLight()
{
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	

	vec3 normalEye = normalize(normalMatrix * normal);	
	
	vec3 lightPosEyeN = normalize(lightPosEye.xyz - fragPosEye.xyz);
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	vec3 halfVector = normalize(lightPosEyeN + viewDirN);

	//compute distance to light
	float dist = length(lightPosEye.xyz - fragPosEye.xyz);
	//compute attenuation
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));


	ambient = att * ambientStrength * lightColor;

	diffuse = att * max(dot(normalEye, lightPosEyeN), 0.0f) * lightColor;
	
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular = att * specularStrength * specCoeff * lightColor;

	ambient *= texture(diffuseTexture, fragTexCoords);
	diffuse *= texture(diffuseTexture, fragTexCoords);
	specular *= texture(specularTexture, fragTexCoords);

	return min((ambient + (1.0f - shadowP) * diffuse) + (1.0f - shadowP) * specular , 1.0f);
}

void main() 
{
	vec3 color = vec3(0.0f);

	shadowD = computeShadow(lightDir);
	shadowP = computeShadow(lightDir);

	if(activeDirL==1)
		color += dirLight();
	if(activePointL==1)
		color += pointLight();
    
    fColor = vec4(color, 1.0f);
}
