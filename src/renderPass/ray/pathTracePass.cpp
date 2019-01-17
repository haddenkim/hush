#include "pathTracePass.h"

#include "camera/camera.h"
#include "interaction/surfaceInteraction.h"
#include "light/light.h"
#include "light/lightSample.h"
#include "material/material.h"
#include "material/materialSample.h"
#include "math/vector.h"
#include "sampler/sampler.h"
#include "sampler/sampling.h"
#include "scene/mesh.h"
#include "scene/scene.h"
#include "texture/texture.h"
#include <tbb/tbb.h>
#include <vector>

#include "pipeline/pipeline.h"
#include "pipelineBuffer/spectrumBuffer.h"

#define TILE_SIZE 8
#define RAY_EPSILON 1e-5

PathTracePass::PathTracePass(Pipeline* pipeline)
	: RayPass("Path Trace",
			  RT_FULL_GI,
			  { CAM_POSITION, CAM_DIRECTION, MESH }, // inputs
			  { RT_COLOR })							 // outputs
	, m_scene(pipeline->m_scene)
	, m_camera(pipeline->m_camera)
	, m_width(pipeline->m_width)
	, m_height(pipeline->m_height)
	, m_numTilesX((pipeline->m_width + TILE_SIZE - 1) / TILE_SIZE)
	, m_numTilesY((pipeline->m_height + TILE_SIZE - 1) / TILE_SIZE)
	, m_rtColorBuffer(static_cast<SpectrumBuffer*>(pipeline->getOrCreateBuffer(RT_COLOR, true)))
{

	// initial settings
	m_samplesPerPixel = 1;
	m_jitterPrimaryRays = false;
	m_directLightStrategy = UNIFORM_ONE;
	m_samplesPerLight = 1;
	m_maxDepth = 1;
	m_minContribution = 0.1f;
}

void PathTracePass::render()
{

	// render tiles in parallel
	tbb::parallel_for(size_t(0), size_t(m_numTilesX * m_numTilesY), [&](size_t i) {
		renderTile(i);
	});

	// flag buffer as dirty
	m_rtColorBuffer->m_isReadyToDisplay = false;
}

void PathTracePass::renderTile(const uint index)
{
	/* calculate tile dimensions and pixels */
	const uint tileY = index / m_numTilesX;								   // tile x index
	const uint tileX = index - tileY * m_numTilesX;						   // tile y index
	const uint x0 = tileX * TILE_SIZE;									   // start x
	const uint x1 = std::min(x0 + TILE_SIZE, m_width);					   // end x
	const uint y0 = tileY * TILE_SIZE;									   // start y
	const uint y1 = std::min(y0 + TILE_SIZE, m_height);					   // end y
	const uint samplesPerTile = TILE_SIZE * TILE_SIZE * m_samplesPerPixel; // may be less if render dimensions are not divisible by tile sizeÎ

	/* data containers */
	RTCRayHit rayHits[samplesPerTile];
	std::vector<Sampler> samplers; // using vector to avoid default sampler constructor
	samplers.reserve(samplesPerTile);

	/* intialize data */
	int rh = 0; // index of the rayHits array
	for (size_t y = y0; y < y1; y++) {
		for (size_t x = x0; x < x1; x++) {
			for (size_t s = 0; s < m_samplesPerPixel; s++) {
				// create RNG sampler
				samplers[rh] = Sampler(x, y, s);
				// setup primary rays
				setupPrimaryRay(x, y, rayHits[rh], samplers[rh]);
				rh++;
			}
		}
	}

	/* set intersect context */
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	context.flags = RTC_INTERSECT_CONTEXT_FLAG_COHERENT;

	/* trace stream of rays */
	rtcIntersect1M(m_scene->m_embScene, &context, &rayHits[0], rh, sizeof(RTCRayHit));

	/* process each ray */
	rh = 0;
	uint bufferIndex;

	for (size_t y = y0; y < y1; y++) {
		bufferIndex = (y * m_width + x0);

		for (size_t x = x0; x < x1; x++) {
			Spectrum color(0.f);

			for (size_t s = 0; s < m_samplesPerPixel; s++) {

				color += renderPixel(rayHits[rh], samplers[rh], s, bufferIndex);
				rh++;
			}

			/* pixel color is average of samples */
			color /= (float)m_samplesPerPixel;

			/* write to buffer */
			m_rtColorBuffer->m_data[bufferIndex] = color;

			bufferIndex++;
		}
	}
}

Spectrum PathTracePass::renderPixel(RTCRayHit& rayHit, Sampler& sampler, const uint pixelSample, const uint bufferIndex)
{
	Spectrum L(0.0f);	// color
	Spectrum beta(1.0f); // weight

	for (int bounce = 0; bounce <= m_maxDepth; bounce++) {
		/* intersect ray with scene and store intersection in isect */
		// first bounce (primary ray) intersect is performed in calling function
		if (bounce != 0) {
			RTCIntersectContext context;
			rtcInitIntersectContext(&context);
			rtcIntersect1(m_scene->m_embScene, &context, &rayHit);
		}

		/* terminate path if ray missed */
		if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
			return L;
		}

		// store intersection data
		SurfaceInteraction surfaceInteraction(rayHit, *m_scene);

		// CODEHERE - investigate if g-buffer should be averaged across all samples or some other strategy
		/* fill g-buffer for first pixel sample */
		// if (pixelSample == 0 && bounce == 0) {
		// 	fillAdditionalBuffers(surfaceInteraction, bufferIndex);
		// }

		/* possibly add emmitted light */
		if (bounce == 0 && surfaceInteraction.m_light) {
			L += surfaceInteraction.m_light->le();
		}

		/* direct lighting */
		Spectrum Ld(0.f);
		switch (m_directLightStrategy) {
		case UNIFORM_ONE:
			Ld = uniformSampleOneLight(surfaceInteraction, sampler);
			break;

		case UNIFORM_ALL:
			Ld = uniformSampleAllLights(surfaceInteraction, sampler);
			break;

		default:
			assert(!"The default case of switch was reached.");
			break;
		}
		L += beta * Ld;

		// CODEHERE - save first bounce direct light to direct light framebuffer
		// CODEHERE - save all subsequent light contribution to indirect light framebuffer

		// CODEHERE - allow multiple, GUI variable bounces per ray. attempt to do so without recursion
		/* indirect lighting */
		MaterialSample matSample = surfaceInteraction.m_material->sample(surfaceInteraction, sampler);
		if (matSample.m_albedo.isBlack() || matSample.m_pdf == 0.f) {
			// indirect bounce will not contribute light
			return L;
		}

		// CODEHERE - implement russian roulette termination and beta adjustment
		/* terminate indirect ray if weight is below min contribution */
		beta *= matSample.m_albedo * AbsDot(matSample.m_direction, surfaceInteraction.m_normalShade) / matSample.m_pdf;
		if (MaxComponent(beta) < m_minContribution) {
			return L;
		}

		// setup next secondary ray
		setupIntersectRay(surfaceInteraction.m_position, matSample.m_direction, rayHit);
	}

	return L;
}

Spectrum PathTracePass::uniformSampleAllLights(SurfaceInteraction& surfaceInteraction, Sampler& sampler)
{
	Spectrum L(0.f);
	for (Light* light : m_scene->m_enabledLightList) {

		// skip light if surface point is light's mesh
		if (light == surfaceInteraction.m_light) {
			continue;
		}

		L += estimateDirect(surfaceInteraction, *light, sampler);
	}

	return L;
}

Spectrum PathTracePass::uniformSampleOneLight(SurfaceInteraction& surfaceInteraction, Sampler& sampler)
{
	// early terminate if no lights
	int numLights = m_scene->m_enabledLightList.size();
	if (numLights == 0) {
		return Spectrum(0.f);
	}

	// early terminate if surface's mesh is on the only light
	if (numLights == 1 && surfaceInteraction.m_light != nullptr) {
		return Spectrum(0.f);
	}

	/* randomly choose a single light to sample */
	int lightNum = sampler.getOneOf(numLights);
	Light* light = m_scene->m_enabledLightList[lightNum];

	return estimateDirect(surfaceInteraction, *light, sampler) //
		* (float)numLights;									   // pdf of choosing light source is 1/numLights, so radiance/pdf = radiance * numLights
}

Spectrum PathTracePass::estimateDirect(SurfaceInteraction& surfaceInteraction, const Light& light, Sampler& sampler)
{
	Spectrum L(0.f);

	for (size_t i = 0; i < m_samplesPerLight; i++) {

		L += sampleLight(surfaceInteraction, light, sampler);
		if (!light.m_isDelta) {
			L += sampleBsdf(surfaceInteraction, light, sampler);
		}
	}

	return L / (float)m_samplesPerLight;
}

Spectrum PathTracePass::sampleLight(SurfaceInteraction& surfaceInteraction, const Light& light, Sampler& sampler)
{
	// sample the light
	LightSample lightSample = light.sampleLi(surfaceInteraction.m_position, sampler);

	if (lightSample.m_pdf == 0.f || lightSample.m_radiance.isBlack()) {
		return Spectrum(0.f);
	}

	// update isect
	surfaceInteraction.m_wi = lightSample.m_direction;

	// evaluate BSDF for light sample
	float bsdfPdf = surfaceInteraction.m_material->pdf(surfaceInteraction);
	Spectrum bsdfReflectance = surfaceInteraction.m_material->evaluate(surfaceInteraction)
		* AbsDot(surfaceInteraction.m_wi, surfaceInteraction.m_normalShade);

	if (bsdfPdf == 0.f || bsdfReflectance.isBlack()) {
		return Spectrum(0.f);
	}

	// compute effect of visibility for light sample
	if (testNotOcclusion(surfaceInteraction.m_position, lightSample.m_direction, lightSample.m_distance) == false) {
		return Spectrum(0.f);
	}

	// add light contribution to radiance
	if (light.m_isDelta) {
		return bsdfReflectance * lightSample.m_radiance / lightSample.m_pdf;
	} else {
		float weight = PowerHeuristic(1, lightSample.m_pdf, 1, bsdfPdf);
		return bsdfReflectance * lightSample.m_radiance * weight / lightSample.m_pdf;
	}
}

Spectrum PathTracePass::sampleBsdf(SurfaceInteraction& surfaceInteraction, const Light& light, Sampler& sampler)
{
	// sample the material
	MaterialSample matSample = surfaceInteraction.m_material->sample(surfaceInteraction, sampler);
	// matSample.m_albedo *= AbsDot(matSample.m_direction, surfaceInteraction.m_normalShade);

	if (matSample.m_pdf == 0.f || matSample.m_albedo.isBlack()) {
		return Spectrum(0.f);
	}

	// update isect
	surfaceInteraction.m_wi = matSample.m_direction;

	/* evaluate light for BSDF sample */
	// perform intersect test
	RTCRayHit rayHit;
	setupIntersectRay(surfaceInteraction.m_position, surfaceInteraction.m_wi, rayHit);
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	rtcIntersect1(m_scene->m_embScene, &context, &rayHit);

	// ray missed
	if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
		return Spectrum(0.f);
	}

	Mesh* hitMesh = m_scene->m_meshList[rayHit.hit.geomID];
	Light* hitLight = hitMesh->m_light; // nullptr if mesh is not a light

	// ray did not hit the light source
	if (hitLight != &light) {
		return Spectrum(0.f);
	}

	// construct isect
	SurfaceInteraction lightSurfaceInteraction = SurfaceInteraction(rayHit, *hitMesh);

	// CODEHERE - refactor light evaluation and pdf to light class
	// evaluate light for material sample
	float distance = glm::distance(surfaceInteraction.m_position, lightSurfaceInteraction.m_position);
	float cosD = glm::dot(lightSurfaceInteraction.m_normalShade, lightSurfaceInteraction.m_wo);
	float lightPdf = (distance * distance) * hitMesh->m_invSurfaceArea / cosD;
	Spectrum lightRadiance = cosD > 0.f ? light.le() : Spectrum(0.f);
	if (lightPdf == 0.f || lightRadiance.isBlack()) {
		return Spectrum(0.f);
	}

	float weight = PowerHeuristic(1, matSample.m_pdf, 1, lightPdf);
	return matSample.m_albedo * lightRadiance * weight / matSample.m_pdf;
}

void PathTracePass::setupPrimaryRay(const uint x, const uint y, RTCRayHit& rayHit, Sampler& sampler)
{
	// ray origin
	rayHit.ray.org_x = m_camera->m_position.x;
	rayHit.ray.org_y = m_camera->m_position.y;
	rayHit.ray.org_z = m_camera->m_position.z;

	// CODEHERE - sample ray direction about the pixel square
	// for now, x and y of each sample is fixed and identical

	int bufferIndex = ((y * m_width) + x);
	rayHit.ray.dir_x = m_camera->m_directionBuffer[bufferIndex].x;
	rayHit.ray.dir_y = m_camera->m_directionBuffer[bufferIndex].y;
	rayHit.ray.dir_z = m_camera->m_directionBuffer[bufferIndex].z;

	// common
	rayHit.ray.tnear = 0;
	rayHit.ray.tfar = HUGE_VALF;
	rayHit.ray.mask = 1;
	// hit
	rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;
}

void PathTracePass::setupIntersectRay(const Vec3f& origin, const Vec3f& direction, RTCRayHit& rayHit)
{
	// ray origin
	rayHit.ray.org_x = origin.x;
	rayHit.ray.org_y = origin.y;
	rayHit.ray.org_z = origin.z;

	// ray direction
	rayHit.ray.dir_x = direction.x;
	rayHit.ray.dir_y = direction.y;
	rayHit.ray.dir_z = direction.z;

	// common
	rayHit.ray.tnear = RAY_EPSILON;
	rayHit.ray.tfar = HUGE_VALF;
	rayHit.ray.mask = 1;

	// hit
	rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;
}

void PathTracePass::setupOcclusionRay(const Vec3f& origin, const Vec3f& direction, const float tFar, RTCRay& pRay)
{
	// ray origin
	pRay.org_x = origin.x;
	pRay.org_y = origin.y;
	pRay.org_z = origin.z;

	// ray direction
	pRay.dir_x = direction.x;
	pRay.dir_y = direction.y;
	pRay.dir_z = direction.z;

	// common
	pRay.tnear = RAY_EPSILON;
	pRay.tfar = tFar - RAY_EPSILON;
	pRay.mask = -1;
}

bool PathTracePass::testNotOcclusion(const Vec3f& origin, const Vec3f& direction, const float tFar)
{
	RTCRay shadowRay;
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	setupOcclusionRay(origin, direction, tFar, shadowRay);
	rtcOccluded1(m_scene->m_embScene, &context, &shadowRay);
	return shadowRay.tfar > 0;
}

bool PathTracePass::guiEdit()
{
}