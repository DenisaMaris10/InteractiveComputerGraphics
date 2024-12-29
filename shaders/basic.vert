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
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
	
	//gl_Position = projection * (vec4(iPos.x, 0, iPos.y, 1.0f) + vec4(vPosition, 1.0f));
	//gl_Position = vec4(0, 0, 0.1f, 1.0f) + vec4(vPosition, 0);
	//gl_Position = vec4(0,0,0.3f, 0.f) + vec4(1*vPosition, 1.0f);

	//gl_Position = projection * view * vec4((cameraPos + vec3(iPos.x, time%5, iPos.y) + vPosition), 1.0f);

	fPosition = vPosition;
	fNormal = vNormal;
	fTexCoords = vTexCoords;
}
