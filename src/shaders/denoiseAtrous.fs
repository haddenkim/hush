// Implementation of
// Dammertz 2010
// Edge-Avoiding À-Trous Wavelet Transform for fast Global Illumination Filtering

#version 330 core

out vec3 FragColor;
in vec2 TexCoords;

uniform sampler2D gColor;
uniform sampler2D gPosition;
uniform sampler2D gNormal;

uniform float resolution;
uniform float colorPhi;
uniform float positionPhi;
uniform float normalPhi;
uniform float stepwidth;

// clang-format off
const float kernel[25] = float[25](
	1.0/256.0, 1.0/64.0, 3.0/128.0, 1.0/64.0, 1.0/256.0,
	1.0/64.0,  1.0/16.0, 3.0/32.0,  1.0/16.0, 1.0/64.0,
	3.0/128.0, 3.0/32.0, 9.0/64.0,  3.0/32.0, 3.0/128.0,
	1.0/64.0,  1.0/16.0, 3.0/32.0,  1.0/16.0, 1.0/64.0,
	1.0/256.0, 1.0/64.0, 3.0/128.0, 1.0/64.0, 1.0/256.0 ); // B3 spline interpolation

const ivec2 offset[25] = ivec2[25]( 
	ivec2(-2,-2), ivec2(-1,-2), ivec2(0,-2), ivec2(1,-2), ivec2(2,-2), 
	ivec2(-2,-1), ivec2(-1,-1), ivec2(0,-2), ivec2(1,-1), ivec2(2,-1), 
	ivec2(-2, 0), ivec2(-1, 0), ivec2(0, 0), ivec2(1, 0), ivec2(2, 0), 
	ivec2(-2, 1), ivec2(-1, 1), ivec2(0, 1), ivec2(1, 1), ivec2(2, 1),
	ivec2(-2, 2), ivec2(-1, 2), ivec2(0, 2), ivec2(1, 2), ivec2(2, 2) );
// clang-format on

void main()
{
	vec3 sum = vec3(0.0);
	float cum_w = 0.0;
	vec2 step = vec2(1. / resolution, 1. / resolution); // texel step size

	vec3 cval = texture(gColor, TexCoords).rgb;
	vec3 pval = texture(gPosition, TexCoords).rgb;
	vec3 nval = texture(gNormal, TexCoords).rgb;

	for (int i = 0; i < 25; i++) {
		vec2 uv = TexCoords + offset[i] * step * stepwidth;

		vec3 ctmp = texture(gColor, uv).rgb;			// Iq			(color of q)
		vec3 t = cval - ctmp;							// Ip - Iq		(color difference)
		float dist2 = dot(t, t);						// ||Ip - Iq||	(distance squared)
		float c_w = min(exp(-(dist2) / colorPhi), 1.0); // w(p,q)		(weight function)

		vec3 ntmp = texture(gNormal, uv).rgb;
		t = nval - ntmp;
		dist2 = max(dot(t, t) / (stepwidth * stepwidth), 0.0);
		float n_w = min(exp(-(dist2) / normalPhi), 1.0);

		vec3 ptmp = texture(gPosition, uv).rgb;
		t = pval - ptmp;
		dist2 = dot(t, t);
		float p_w = min(exp(-(dist2) / positionPhi), 1.0);

		float weight = c_w * n_w * p_w;
		sum += ctmp * weight * kernel[i];
		cum_w += weight * kernel[i];
	}

	FragColor = sum / cum_w;
}