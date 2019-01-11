#include "imguiExtensions.h"

bool ImGui::InputVec3f(const char* label, Point3f* p, const char* format, ImGuiInputTextFlags extra_flags)
{
	float vArray[3] = { p->x, p->y, p->z };
	if (ImGui::InputFloat3(label, vArray, format, extra_flags)) {
		p->x = vArray[0];
		p->y = vArray[1];
		p->z = vArray[2];

		return true;
	} else {
		return false;
	}
}

bool ImGui::SliderUint(const char* label, unsigned int* v, unsigned int v_min, unsigned int v_max, const char* format)
{
	return ImGui::SliderScalar(label, ImGuiDataType_U32, v, &v_min, &v_max, format);
}