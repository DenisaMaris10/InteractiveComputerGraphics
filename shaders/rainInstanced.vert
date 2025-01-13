#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

layout(location=3) in vec2 iPos;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;
uniform float time;

void main() 
{

	float height = 14.0f - mod(time + float(gl_InstanceID), 15.0);
	float xPos = iPos.x + cameraPos.x;
	if(height <= 3)
	{
		height = sin(radians(mod(height, 3.0f)*90)) + 0.2;
		if(mod(gl_InstanceID, 2) == 0)
			xPos += 0.5f;
		else 
			xPos -= 0.5f;
	}
	float zPos = iPos.y + cameraPos.z ;


	gl_Position = projection * view * vec4((vec3(xPos, height, zPos) + vPosition), 1.0f);

	fPosition = vPosition;
	fNormal = vNormal;
	fTexCoords = vTexCoords;
}
