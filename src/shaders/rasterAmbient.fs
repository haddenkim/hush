#version 330 core
out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D gAmbient;
uniform float ambientIntensity;

void main()
{
	// retrieve data from gbuffer
	vec3 matAmbient = texture(gAmbient, TexCoords).rgb;

	// scale material by scene's intensity
	FragColor = matAmbient * ambientIntensity;
}