#include "pipeline/pipeline.h"
#include "pipelineBuffer/buffer.h"
#include "renderPass/renderPass.h"

bool Pipeline::guiEdit()
{
	bool wasModified = false;

	for (uint i = 0; i < m_stages.size(); i++) {
		ImGui::Text("Stage %i", i);
		ImGui::Separator();
		PipelineStage& stage = m_stages[i];

		for (PipelinePass& pass : stage.m_passes) {

			RenderPass* renderPass = pass.m_renderPass;

			if (ImGui::TreeNode(renderPass->m_name.c_str())) {

				ImGui::Columns(2);
				ImGui::Separator();

				// column headers
				ImGui::Text("Input");
				ImGui::NextColumn();
				ImGui::Text("Output");
				ImGui::NextColumn();
				ImGui::Separator();

				// input list
				for (uint i = 0; i < renderPass->m_inputs.size(); ++i) {
					if (renderPass->m_inputs[i]) {
						ImGui::Text("%s", PipelineIONames[i]);
					}
				}
				ImGui::NextColumn();

				// output list
				for (uint i = 0; i < renderPass->m_outputs.size(); ++i) {
					if (renderPass->m_outputs[i]) {
						ImGui::Text("%s", PipelineIONames[i]);
					}
				}
				ImGui::NextColumn();

				ImGui::Columns(1);
				ImGui::Separator();

				if (renderPass->guiEdit()) {
					wasModified = true;
				}

				ImGui::TreePop();
			}
		}
		ImGui::Separator();
		ImGui::NewLine();
	}

	return wasModified;
}

void Pipeline::guiFramebuffer()
{
	uint index = 0; // to keep track of which buffer is selected

	for (uint i = 0; i < m_stages.size(); i++) {
		ImGui::Text("\tStage %i", i);
		ImGui::Separator();
		PipelineStage& stage = m_stages[i];

		for (PipelinePass& pass : stage.m_passes) {

			for (Buffer* buffer : pass.m_buffers) {

				std::string text = "(" + pass.m_renderPass->m_name + ") " + PipelineIONames[buffer->m_type];

				if (ImGui::Selectable(text.c_str(), buffer == m_displayBuffer)) {

					// buffer was clicked, so switch it to be displayed
					m_displayBuffer = buffer;
				}
			}
		}
		ImGui::Separator();
		ImGui::NewLine();
	}
}