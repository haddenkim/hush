// Implementation of
// Dammertz 2010
// Edge-Avoiding À-Trous Wavelet Transform for fast Global Illumination Filtering

#version 330 core
out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D dColor;
uniform sampler2D gDiffuse;

void main()
{
	FragColor = texture(dColor, TexCoords).rgb * texture(gDiffuse, TexCoords).rgb;
}