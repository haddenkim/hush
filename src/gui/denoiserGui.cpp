#include "denoiser/atrousDenoiser.h"
#include "gui/imgui/imguiExtensions.h"

bool AtrousDenoiser::guiEdit()
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