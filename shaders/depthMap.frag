#version 410 core

in vec2 fragTexCoords;

out vec4 fColor;

uniform sampler2D diffuseTexture;

void main()
{
	vec4 fragTexture = texture(diffuseTexture, fragTexCoords);
	if(fragTexture.a < 0.1)
		discard;
	
	fColor = vec4(1.0f); 
}