#include "renderPass/rasterGBufferPass.h"
#include "renderPass/ssAmbientPass.h"
#include "renderPass/ssLightPass.h"

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