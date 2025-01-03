#version 410 core

in vec3 textureCoordinates;
out vec4 fColor;
in vec4 fPosEye;

uniform samplerCube skybox;
uniform bool fog;

float computeFog()
{
	float fogDensity = 0.01f;
	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(1000 * fogDensity, 2));
	return clamp(fogFactor, 0.0f, 1.0f);
}

void main()
{
	vec4 color = texture(skybox, textureCoordinates);
	if(fog)
	{
		float fogFactor = computeFog();
		vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
		fColor = fogColor*(1-fogFactor) + color*fogFactor;
	}
	else
		fColor = color;
}
