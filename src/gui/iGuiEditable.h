#pragma once

#include <imgui.h>

class IGuiEditable {

	// returns true if the object was modified. false otherwise
	virtual bool guiEdit() = 0; 

};