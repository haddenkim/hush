#version 330 core
out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D displayTex;

void main()
{
	FragColor = texture(displayTex, TexCoords).rgb;
}