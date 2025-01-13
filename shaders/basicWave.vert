#version 410 core

layout(location=0) in vec2 vTexCoords;
layout(location=1) in vec2 gridXY;

out vec2 fTexCoords;
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
uniform mat3 normalMatrix;

uniform	vec3 dirLightDir;
uniform	vec3 posLightDir1;
uniform	vec3 posLightDir2;
uniform	vec3 posLightDir3;
uniform	vec3 posLightDir4;
uniform mat4 lightSpaceTrMatrix;
uniform	vec3 spotLightPos;
uniform vec3 spotLightDirection;

uniform float time;
uniform vec2 gridDimensions;
uniform vec2 gridSize;

void main() 
{
	fTexCoords = vTexCoords;

	// compute uv coordinates (le aducem in intervalul [-1, 1])
	float u = gridXY.x / (gridSize.x - 1);
	float v = gridXY.y / (gridSize.y - 1);

	u = u * 2.0f - 1.0f;
	v = v * 2.0f - 1.0f;

	// compute simple sine wave pattern y coordinate
	float freq = 7.0f;
	float ampl = 0.25f;

	//compute position
	//evaluate the function of the sine surface (f(u,v) = x * (1, 0, 0) + y * (0, 1, 0) + z * (0, 0, 1))
	float x = - (gridDimensions.x / 2.0f) + (u + 1.0f) / 2.0f * gridDimensions.x;
	float y = - 1.0f + sin(u * freq + time) * cos(v * freq + time) * ampl; // 1.2f vine de la coordonata din Blender a planului cu apa (axa z)
	float z = - (gridDimensions.y / 2.0f) + (v + 1.0f) / 2.0f * gridDimensions.y;

	//compute tangent (partial derivative of f over u)
	vec3 tangent = vec3(gridDimensions.x / 2.0f, ampl * freq * cos(v * freq + time) * cos(u * freq + time), 0.0f);

	//compute bi-tangent (partial derivative of f over v)
	vec3 bitangent = vec3(0.0f, - ampl * freq * sin(u * freq + time) * sin(v * freq + time), gridDimensions.y / 2.0f);

	//compute normal
	fNormal = normalize(normalMatrix * cross(bitangent, tangent));

	//compute position in eye space
	fPosEye = view * model * vec4(x, y, z, 1.0f);
	directionalLightPosEye = view * vec4(dirLightDir, 0.0f);
	positionalLightPosEye1 = view * vec4(posLightDir1, 1.0f);
	positionalLightPosEye2 = view * vec4(posLightDir2, 1.0f);
	positionalLightPosEye3 = view * vec4(posLightDir3, 1.0f);
	positionalLightPosEye4 = view * vec4(posLightDir4, 1.0f);
	spotLightPosEye = view * vec4(spotLightPos, 1.0f);
	spotLightDirEye = view * vec4(spotLightDirection, 0.0f);

	gl_Position = projection * view * model * vec4(x, y, z, 1.0f);
}
