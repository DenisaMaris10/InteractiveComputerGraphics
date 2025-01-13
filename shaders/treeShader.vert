#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fNormal;
out vec4 fPosEye;
out vec4 directionalLightPosEye;
out vec2 fragTexCoords;
// directiile luminilor pozitionale
out vec4 positionalLightPosEye1; 
out vec4 positionalLightPosEye2; 
out vec4 positionalLightPosEye3; 
out vec4 positionalLightPosEye4; 
out vec4 spotLightPosEye; 
out vec4 spotLightDirEye; 
out vec4 fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	mat3 normalMatrix;
uniform	vec3 dirLightDir;
uniform	vec3 posLightDir1;
uniform	vec3 posLightDir2;
uniform	vec3 posLightDir3;
uniform	vec3 posLightDir4;
uniform	vec3 spotLightPos;
uniform vec3 spotLightDirection;
uniform mat4 lightSpaceTrMatrix;

void main() 
{
	//compute eye space coordinates
	fragTexCoords = vTexCoords;
	fPosEye = view * model * vec4(vPosition, 1.0f);
	directionalLightPosEye = view * vec4(dirLightDir, 0.0f);
	positionalLightPosEye1 = view * vec4(posLightDir1, 1.0f);
	positionalLightPosEye2 = view * vec4(posLightDir2, 1.0f);
	positionalLightPosEye3 = view * vec4(posLightDir3, 1.0f);
	positionalLightPosEye4 = view * vec4(posLightDir4, 1.0f);
	spotLightPosEye = view * vec4(spotLightPos, 1.0f);
	spotLightDirEye = view * vec4(spotLightDirection, 0.0f);
	fNormal = normalize(normalMatrix * vNormal);

	fragPosLightSpace = lightSpaceTrMatrix * model * vec4(vPosition, 1.0f);

	gl_Position = projection * view * vec4(vPosition, 1.0f);
}
