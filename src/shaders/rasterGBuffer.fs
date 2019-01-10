#version 330 core
struct Material {
	vec3 ambient;
	vec3 emission;
	vec3 diffuse;
	vec3 specular;
	float shininess;

	bool hasTexDiffuse;
	sampler2D texDiffuse;
};

layout(location = 0) out vec4 gSpecular;
layout(location = 1) out vec3 gDiffuse;
layout(location = 2) out vec3 gPosition;
layout(location = 3) out vec3 gNormal;
layout(location = 4) out vec3 gAmbient;

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform Material material;

void main()
{
	// store to buffers
	gPosition = position;
	gNormal = normalize(normal);

	// diffuse color
	if (material.hasTexDiffuse) {
		gDiffuse = texture(material.texDiffuse, uv).rgb;
	} else {
		gDiffuse = material.diffuse;
	}

	// specular color
	gSpecular = vec4(material.specular, material.shininess);

	// ambient color
	gAmbient = material.ambient;
}