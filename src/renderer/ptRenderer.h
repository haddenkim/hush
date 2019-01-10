#pragma once

#include "common.h"
#include "denoiser/atrousDenoiser.h"
#include "renderer.h"
#include <embree3/rtcore.h>
#include <glad/glad.h>

class Mesh;
class Scene;
class Camera;
class SurfaceInteraction;
class Sampler;
class Light;

enum FrameBufferChannel {
	FB_COLOR = (1 << 0),
	FB_POSITION = (1 << 1),
	FB_NORMAL = (1 << 2),
	FB_DIFFUSE = (1 << 3)
};

enum DirectLightStrategy : int {
	UNIFORM_ONE = 0,
	UNIFORM_ALL = 1
};

enum DenoiseStrategy : int {
	NONE = 0,
	ATROUS = 1
};

class PtRenderer : public Renderer {
public:
	PtRenderer(Scene* scene, Camera* camera);

	void render3d() override;
	void renderPostProcess() override;

	// renderer settings
	const uint m_numTilesX;
	const uint m_numTilesY;
	uint m_samplesPerPixel;
	bool m_jitterPrimaryRays;

	DirectLightStrategy m_directLightStrategy;
	uint m_samplesPerLight;

	uint m_maxDepth;
	float m_minContribution;

	void selectDenoiser(DenoiseStrategy strategy);
	DenoiseStrategy m_denoiseStrategy;
	AtrousDenoiser m_atrousDenoiser;

	// opengl
	GLuint* m_gBufferTex;

protected:
	// buffers
	uint m_bufferSize;
	int m_bufferChannels;	// FrameBufferChannel bitmask
	float* m_colorBuffer;	// RGB
	float* m_positionBuffer; // XYZ
	float* m_normalBuffer;   // XYZ
	float* m_diffuseBuffer;  // RGB

	// camera settings
	const float m_fovScale;
	const float m_invWidth;
	const float m_invHeight;

	// rendering helpers
	void setupPrimaryRay(const uint x, const uint y, const Mat4& c2w, RTCRayHit& rayHit, Sampler& sampler);
	void setupIntersectRay(const Vec3f& origin, const Vec3f& direction, RTCRayHit& rayHit);
	void setupOcclusionRay(const Vec3f& origin, const Vec3f& direction, const float tFar, RTCRay& pRay);
	bool testNotOcclusion(const Vec3f& origin, const Vec3f& direction, const float tFar);

	// rendering
	void renderTile(const uint index, const Mat4& c2w);
	Spectrum renderPixel(RTCRayHit& rayHit, Sampler& sampler, const uint pixelSample, const uint bufferIndex);

	Spectrum uniformSampleOneLight(SurfaceInteraction& surfaceInteraction, Sampler& sampler);
	Spectrum uniformSampleAllLights(SurfaceInteraction& surfaceInteraction, Sampler& sampler);
	Spectrum estimateDirect(SurfaceInteraction& surfaceInteraction, const Light& light, Sampler& sampler);
	Spectrum sampleLight(SurfaceInteraction& surfaceInteraction, const Light& light, Sampler& sampler);
	Spectrum sampleBsdf(SurfaceInteraction& surfaceInteraction, const Light& light, Sampler& sampler);

	void clearBuffers(int bufferChannels);
	void fillColorBuffer(const Spectrum& color, const uint bufferIndex);
	void fillAdditionalBuffers(const SurfaceInteraction& surfaceInteraction, const uint bufferIndex);

	// setup helper
	void setupGBuffer();
};