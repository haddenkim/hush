#pragma once

#include "common.h"
#include "denoiser/atrousDenoiser.h"
#include "gui/iGuiEditable.h"
#include "renderer.h"
#include <embree3/rtcore.h>
#include <glad/glad.h>
#include <vector>

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

class PtRenderer : public Renderer, IGuiEditable {
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

	// UI
	bool guiEdit() override;

protected:
	// buffers
	// uint m_bufferSize;
	int m_bufferChannels; // FrameBufferChannel bitmask
	std::vector<Spectrum> m_colorBuffer;
	std::vector<Point3f> m_positionBuffer;
	std::vector<Vec3f> m_normalBuffer;
	std::vector<Spectrum> m_diffuseBuffer;

	// rendering helpers
	void setupPrimaryRay(const uint x, const uint y, RTCRayHit& rayHit, Sampler& sampler);
	void setupIntersectRay(const Vec3f& origin, const Vec3f& direction, RTCRayHit& rayHit);
	void setupOcclusionRay(const Vec3f& origin, const Vec3f& direction, const float tFar, RTCRay& pRay);
	bool testNotOcclusion(const Vec3f& origin, const Vec3f& direction, const float tFar);

	// rendering
	void renderTile(const uint index);
	Spectrum renderPixel(RTCRayHit& rayHit, Sampler& sampler, const uint pixelSample, const uint bufferIndex);

	Spectrum uniformSampleOneLight(SurfaceInteraction& surfaceInteraction, Sampler& sampler);
	Spectrum uniformSampleAllLights(SurfaceInteraction& surfaceInteraction, Sampler& sampler);
	Spectrum estimateDirect(SurfaceInteraction& surfaceInteraction, const Light& light, Sampler& sampler);
	Spectrum sampleLight(SurfaceInteraction& surfaceInteraction, const Light& light, Sampler& sampler);
	Spectrum sampleBsdf(SurfaceInteraction& surfaceInteraction, const Light& light, Sampler& sampler);

	void clearBuffers(int bufferChannels);
	void fillAdditionalBuffers(const SurfaceInteraction& surfaceInteraction, const uint bufferIndex);

	// setup helper
	void setupGBuffer();
};