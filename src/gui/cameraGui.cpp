#include "camera/camera.h"
#include "gui/imgui/imguiExtensions.h"

bool Camera::guiEdit()
{
	bool wasModified = false;

	if (ImGui::MenuItem("Reset")) {
		reset();
		wasModified = true;
	}

	ImGui::Separator();
	const char* modes[] = { "Orbit", "Rotate", "Translate" };
	if (ImGui::Combo("Mode", (int*)&m_mode, modes, IM_ARRAYSIZE(modes))) {
		wasModified = true;
	}

	if (ImGui::InputVec3f("Position", &m_position)) {
		updateMatrices();
		wasModified = true;
	}
	if (ImGui::InputVec3f("Center", &m_center)) {
		updateMatrices();
		wasModified = true;
	}
	if (ImGui::InputVec3f("Up", &m_up)) {
		updateMatrices();
		wasModified = true;
	}

	ImGui::Separator();
	if (ImGui::CollapsingHeader("Camera Controls")) {
		ImGui::Text("All Modes:");
		ImGui::BulletText("Z,X - Zoom");
		ImGui::BulletText("C - Switch Mode");
		ImGui::BulletText("R - Reset");

		ImGui::Text("Orbit & Rotate Mode:");
		ImGui::BulletText("W,S - Pitch");
		ImGui::BulletText("A,D - Yaw");
		ImGui::BulletText("Q,E - Roll");

		ImGui::Text("Translate Mode:");
		ImGui::BulletText("W - Forward");
		ImGui::BulletText("S - Backward");
		ImGui::BulletText("A - Left");
		ImGui::BulletText("D - Right");
		ImGui::BulletText("Q - Up");
		ImGui::BulletText("E - Down");
	}

	return wasModified;
}