#version 330 core
out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D dColor;
uniform sampler2D gDiffuse;

void main()
{
	FragColor = texture(dColor, TexCoords).rgb * texture(gDiffuse, TexCoords).rgb;
}