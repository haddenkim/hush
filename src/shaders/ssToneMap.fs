#version 330 core
out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D inColorTex;

void main()
{
	FragColor = pow(texture(inColorTex, TexCoords).rgb, vec3(1.0 / 2.2));
}