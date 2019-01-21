#include "gui/imgui/imguiExtensions.h"

#include "renderPass/denoise/atrousDenoiserPass.h"
#include "renderPass/post/toneMapPass.h"
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
	bool wasModified = false;

	ImGui::Text("Path Tracer Settings");
	/* film settings */
	if (ImGui::TreeNode("Film")) {
		if (ImGui::SliderUint("SPP", &m_samplesPerPixel, 1, 10)) {
			wasModified = true;
		}

		ImGui::TreePop();
	}

	/* direct light settings */
	if (ImGui::TreeNode("Direct Lighting")) {
		const char* options[] = { "Uniform Sample One", "Uniform Sample All" };
		if (ImGui::Combo("Strategy", (int*)&m_directLightStrategy, options, IM_ARRAYSIZE(options))) {
			wasModified = true;
		}
		if (ImGui::SliderUint("# Samples per Light", &m_samplesPerLight, 1, 10)) {
			wasModified = true;
		}

		ImGui::TreePop();
	}

	/* indirect light settings */
	if (ImGui::TreeNode("Indirect Lighting")) {
		if (ImGui::SliderUint("Max Depth", &m_maxDepth, 0, 10)) {
			wasModified = true;
		}
		if (ImGui::SliderFloat("Min Contribution", &m_minContribution, 0.f, 1.f, "%.2f")) {
			wasModified = true;
		}

		ImGui::TreePop();
	}

	return wasModified;
}

bool AtrousDenoiserPass::guiEdit()
{
	bool wasModified = false;

	if (ImGui::SliderUint("Iterations", &m_filterIterations, 1, 10)) {
		wasModified = true;
	}
	if (ImGui::SliderFloat("Sigma Color", &m_colorSigma, 0.01f, 10.f, "%.2f")) {
		wasModified = true;
	}
	if (ImGui::SliderFloat("Sigma Position", &m_positionSigma, 0.01f, 10.f, "%.2f")) {
		wasModified = true;
	}
	if (ImGui::SliderFloat("Sigma Normal", &m_normalSigma, 0.01f, 10.f, "%.2f")) {
		wasModified = true;
	}
	if (ImGui::Checkbox("Multiply by Diffuse?", &m_useOptionalDiffuse)) {
		wasModified = true;
	}

	return wasModified;
}

bool RenderToScreenPass::guiEdit()
{
}

bool ToneMapPass::guiEdit()
{
}
