#include "renderPass/denoise/atrousDenoiserPass.h"
#include "renderPass/raster/rasterGBufferPass.h"
#include "renderPass/raster/ssAmbientPass.h"
#include "renderPass/raster/ssLightPass.h"
#include "renderPass/ray/pathTracePass.h"
#include "renderPass/renderToScreenPass.h"

bool RasterGBufferPass::guiEdit()
{
	bool wasModified = false;

	return wasModified;
}

bool SsLightPass::guiEdit()
{
	bool wasModified = false;

	return wasModified;
}

bool SsAmbientPass::guiEdit()
{
	bool wasModified = false;

	// float m_ambientIntensity;
	// bool m_useDiffuse;

	if (ImGui::SliderFloat("Intensity", &m_ambientIntensity, 0.f, 10.f, "%.1f")) {
		wasModified = true;
	}

	ImGui::Checkbox("Use diffuse material?", &m_useDiffuse);

	return wasModified;
}

bool PathTracePass::guiEdit()
{
}

bool AtrousDenoiserPass::guiEdit()
{
}

bool RenderToScreenPass::guiEdit()
{
}
