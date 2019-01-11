#include "gui/imgui/imguiExtensions.h"
#include "renderer/ptRenderer.h"

bool PtRenderer::guiEdit()
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

	ImGui::Separator();
	ImGui::NewLine();

	ImGui::Text("Post Process Settings");
	const char* options[] = { "None", "Atrous" };
	if (ImGui::Combo("Strategy", (int*)&m_denoiseStrategy, options, IM_ARRAYSIZE(options))) {
		selectDenoiser(m_denoiseStrategy);
		wasModified = true;
	}

	if (m_denoiseStrategy == ATROUS) {
		if (m_atrousDenoiser.guiEdit()) {
			wasModified = true;
		}
	}

	return wasModified;
}