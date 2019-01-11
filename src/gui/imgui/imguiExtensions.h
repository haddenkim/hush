#include "common.h"
#include <imgui.h>

namespace ImGui {

bool InputVec3f(const char* label, Point3f* p, const char* format = "%.3f", ImGuiInputTextFlags extra_flags = 0);

bool SliderUint(const char* label, unsigned int* v, unsigned int v_min, unsigned int v_max, const char* format = "%u");
}
