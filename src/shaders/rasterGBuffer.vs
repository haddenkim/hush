#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 VP;
// uniform mat4 M;

out vec3 position;
out vec3 normal;
out vec2 uv;

void main()
{
	position = aPos;
	normal = aNormal;
	uv = aUV;

	gl_Position = VP * vec4(aPos.xyz, 1.0);

}