#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec4 directionalLightPosEye; // lightDir*view
in vec4 positionalLightPosEye1; 
in vec4 positionalLightPosEye2; 
in vec4 positionalLightPosEye3; 
in vec4 positionalLightPosEye4; 
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

//lighting
uniform	vec3 lightColor;
uniform vec3 positionalLightColor1;
uniform sampler2D shadowMap;
uniform vec3 positionalLightColor2;
uniform vec3 positionalLightColor3;
uniform vec3 positionalLightColor4;
uniform sampler2D diffuseTexture;
//uniform sampler2D specularTexture;
uniform bool fog;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

float constant = 1.0f;
float linear = 0.0045f;
float quadratic = 0.0075f;

// indicii specifici fiecarui tip de lumina
vec3 ambientPos1, ambientPos2, ambientPos3, ambientPos4;
vec3 diffusePos1, diffusePos2, diffusePos3, diffusePos4;
vec3 specularPos1, specularPos2, specularPos3, specularPos4;

void computeDirLightComponents(inout vec3 ambient, inout vec3 diffuse, inout vec3 specular, vec3 lightColor, vec4 lightPosEye)
{
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(vec3(lightPosEye)).xyz;

	//compute view direction 
	vec3 viewDirN = normalize(fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 halfVector = normalize(lightDirN + viewDirN);
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;


}

void computePositionalLight(inout vec3 ambient, inout vec3 diffuse, inout vec3 specular, vec3 lightColor, vec4 lightDir)
{
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(vec3(lightDir) - fPosEye.xyz).xyz;
	

	//compute view direction 
	vec3 viewDirN = normalize(-fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	//vec3 reflection = reflect(-lightDirN, normalEye);
	vec3 halfVector = normalize(lightDirN + viewDirN);
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;

	//compute distance to light
	float dist = length(vec3(lightDir) - fPosEye.xyz);
	//compute attenuation
	float att = 1.0f/(constant + linear * dist + quadratic *(dist*dist));
	
	//compute ambient light
	ambient = att*ambient;

	//compute diffuse light
	diffuse = att*diffuse;

	specular = att*specular;
}

float computeShadow()
{
	// perform perspective divide
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	// Transform to [0,1] range
	normalizedCoords = normalizedCoords * 0.5 + 0.5;

	// Get closest depth value from light's perspective
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;

	// Get depth of current fragment from light's perspective
	float currentDepth = normalizedCoords.z;

	// Check whether current frag pos is in shadow
	//float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

	// Check whether current frag pos is in shadow
	float bias = max(0.05f * (1.0f - dot(fNormal, directionalLightPosEye.xyz)), 0.005f);
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

	if(normalizedCoords.z > 1.0f)
		return 0.0f;

	return shadow;
}

float computeFog()
{
	float fogDensity = 0.01f;
	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
	return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
	// calculam lumina directionala
	//computeDirLightComponents(ambient, diffuse, specular, lightColor, directionalLightPosEye);
	// calculam luminile pozitionale
	computePositionalLight(ambientPos1, diffusePos1, specularPos1, positionalLightColor1, positionalLightPosEye1);
	computePositionalLight(ambientPos2, diffusePos2, specularPos2, positionalLightColor2, positionalLightPosEye2);
	computePositionalLight(ambientPos3, diffusePos3, specularPos3, positionalLightColor3, positionalLightPosEye3);
	computePositionalLight(ambientPos4, diffusePos4, specularPos4, positionalLightColor4, positionalLightPosEye4);

	ambient += ambientPos1 + ambientPos2 + ambientPos3 + ambientPos4;
	diffuse += diffusePos1 + diffusePos2 + diffusePos3 + diffusePos4;
	specular += specularPos1 + specularPos2 + specularPos3 + specularPos4;
	
	//pentru harti difuze
	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
	if(colorFromTexture.a < 0.1)
		discard;
	else{
	ambient *= colorFromTexture.xyz; //texture(diffuseTexture, fTexCoords);
	diffuse *= colorFromTexture.xyz; //texture(diffuseTexture, fTexCoords);
	//specular *= texture(specularTexture, fTexCoords);
	}

	float shadow = computeShadow();
	vec3 color = min((ambient + (1.0f - shadow) * diffuse) + (1.0f - shadow) * specular, 1.0f);

	if(fog)
	{
		float fogFactor = computeFog();
		vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
		fColor = fogColor*(1-fogFactor) + vec4(color, colorFromTexture.a)*fogFactor;
	}
	else
		fColor = vec4(color, colorFromTexture.a);


 //colorFromTexture = texture(diffuseTexture, vec2(0.8, 0.9));
	//fColor = colorFromTexture;
	//fColor = vec4(1, 0, 0, 1);
}
