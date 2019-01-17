#pragma once
#include "renderPass/renderPass.h"
#include <embree3/rtcore.h>

class Scene;
class Camera;
class Buffer;
class SpectrumBuffer;
class SurfaceInteraction;
class Sampler;
class Light;

enum DirectLightStrategy : int {
	UNIFORM_ONE = 0,
	UNIFORM_ALL = 1
};

class RtFullGiPass : public RenderPass {
public:
	RtFullGiPass(Scene* scene,
				 Camera* camera,

				 Buffer* rtColorBuffer);

	void render() override;

	// UI
	bool guiEdit() override;
	// renderer settings

	uint m_samplesPerPixel;
	bool m_jitterPrimaryRays;

	DirectLightStrategy m_directLightStrategy;
	uint m_samplesPerLight;

	uint m_maxDepth;
	float m_minContribution;

protected:
	// render helpers
	void renderTile(const uint index);
	Spectrum renderPixel(RTCRayHit& rayHit, Sampler& sampler, const uint pixelSample, const uint bufferIndex);

	void setupPrimaryRay(const uint x, const uint y, RTCRayHit& rayHit, Sampler& sampler);
	void setupIntersectRay(const Vec3f& origin, const Vec3f& direction, RTCRayHit& rayHit);
	void setupOcclusionRay(const Vec3f& origin, const Vec3f& direction, const float tFar, RTCRay& pRay);
	bool testNotOcclusion(const Vec3f& origin, const Vec3f& direction, const float tFar);

	Spectrum uniformSampleOneLight(SurfaceInteraction& surfaceInteraction, Sampler& sampler);
	Spectrum uniformSampleAllLights(SurfaceInteraction& surfaceInteraction, Sampler& sampler);
	Spectrum estimateDirect(SurfaceInteraction& surfaceInteraction, const Light& light, Sampler& sampler);
	Spectrum sampleLight(SurfaceInteraction& surfaceInteraction, const Light& light, Sampler& sampler);
	Spectrum sampleBsdf(SurfaceInteraction& surfaceInteraction, const Light& light, Sampler& sampler);

	// inputs
	Scene* m_scene;
	Camera* m_camera;

	// data, state
	const uint m_width;
	const uint m_height;
	const uint m_numTilesX;
	const uint m_numTilesY;

	// outputs
	SpectrumBuffer* m_rtColorBuffer;
	// Buffer* m_normalBuffer;
	// Buffer* m_matAmbientBuffer;
	// Buffer* m_matDiffuseBuffer;
	// Buffer* m_matSpecularBuffer;
};