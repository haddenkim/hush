#include "pipeline/pipeline.h"
#include "pipelineBuffer/buffer.h"
#include "renderPass/renderPass.h"

bool Pipeline::guiEdit()
{
	bool wasModified = false;

	for (RenderPass* renderPass : m_passes) {

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
					ImGui::Text("%s",PipelineIONames[i]);
				}
			}
			ImGui::NextColumn();

			// output list
			for (uint i = 0; i < renderPass->m_outputs.size(); ++i) {
				if (renderPass->m_outputs[i]) {
					ImGui::Text("%s",PipelineIONames[i]);
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

	return wasModified;
}

void Pipeline::guiFramebuffer()
{
	for (uint i = 0; i < m_bufferManager.size(); i++) {
		Buffer* buffer = m_bufferManager.at(i);
		
		std::string title (PipelineIONames[buffer->m_type] );
		title += buffer->m_hardware == GPU ? " gpu" : " cpu";

		ImGui::Selectable(title.c_str(), m_displayedBufferIndex == i);
	}
}