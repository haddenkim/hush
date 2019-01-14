#version 330 core
struct Light {
	vec3 position;
	vec3 intensity;
};

out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D gSpecular;
uniform sampler2D gDiffuse;
uniform sampler2D gPosition;
uniform sampler2D gNormal;

uniform Light light;
uniform vec3 cameraPosition;

void main()
{
	// retrieve data from gbuffer
	vec3 worldPos = texture(gPosition, TexCoords).rgb;
	vec3 hitNormal = texture(gNormal, TexCoords).rgb;
	vec3 matDiffuse = texture(gDiffuse, TexCoords).rgb;
	vec3 matSpecular = texture(gSpecular, TexCoords).rgb;
	float matShininess = texture(gSpecular, TexCoords).a;

	// compute common variables
	vec3 lightDir = normalize(light.position - worldPos);
	float lDotN = max(dot(lightDir, hitNormal), 0.0f);
	float lightDistance = length(light.position - worldPos);
	vec3 lightRadianceAttenuated = light.intensity / (lightDistance * lightDistance);

	// Diffuse term
	vec3 diffuseColor = matDiffuse * lDotN * lightRadianceAttenuated;

	// Specular term
	vec3 camDir = normalize(cameraPosition - worldPos);
	vec3 reflectDir = normalize(reflect(-lightDir, hitNormal));
	vec3 specularColor = matSpecular * pow(max(dot(reflectDir, camDir), 0.0), matShininess) * lightRadianceAttenuated;

	// phong shading color
	// FragColor = diffuseColor + specularColor;

	// if shininess is 0, could get pow(0,0) = undefined in GLSL
	if (matShininess == 0) {
		FragColor = diffuseColor;

	} else {
		FragColor = diffuseColor + specularColor;
	}

}