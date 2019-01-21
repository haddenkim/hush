#version 330 core
out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D inColor;
uniform sampler2D gAmbient;
uniform sampler2D gDiffuse;

uniform float ambientIntensity;
uniform bool useDiffuse;

void main()
{
	vec3 matAmbient;

	// retrieve data from buffers
	if (useDiffuse) {
		matAmbient = texture(gDiffuse, TexCoords).rgb;

	} else {
		matAmbient = texture(gAmbient, TexCoords).rgb;
	}

	// scale material by intensity
	FragColor = texture(inColor, TexCoords).rgb + matAmbient * ambientIntensity;
}