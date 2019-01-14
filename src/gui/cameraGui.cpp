#include "camera/camera.h"
#include "gui/imgui/imguiExtensions.h"
#include "scene/scene.h"

bool Camera::guiEdit()
{
	bool wasModified = false;

	if (ImGui::MenuItem("Reset")) {
		reset();
		wasModified = true;
	}
	ImGui::Separator();

	if (ImGui::MenuItem("Anchor at current")) {
		setBase(m_position, m_center, m_up);
		wasModified = true;
	}
	if (ImGui::MenuItem("Anchor at scene center")) {
		setBase(m_scene->m_center,
				m_scene->m_center + Vec3f(0.f, 0.f, 0.1f), // offset a small amount
				Vec3f(0.f, 1.f, 0.f));

		wasModified = true;
	}
	if (ImGui::MenuItem("Anchor at scene +z")) {
		float distance = (m_scene->m_maxBounds.z - m_scene->m_center.z) * 5.19615f; // ... * length (3,3,3)

		setBase(m_scene->m_center + Vec3f(0.f, 0.f, distance),
				m_scene->m_center,
				Vec3f(0.f, 1.f, 0.f));

		wasModified = true;
	}
	if (ImGui::MenuItem("Anchor at scene +xyz")) {
		setBase(m_scene->m_center + (m_scene->m_maxBounds - m_scene->m_center) * 3.0f,
				m_scene->m_center,
				Vec3f(0.f, 1.f, 0.f));

		wasModified = true;
	}

	ImGui::Separator();
	ImGui::NewLine();

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