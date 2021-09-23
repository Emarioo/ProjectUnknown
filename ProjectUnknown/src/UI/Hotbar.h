#pragma once

#include "Engone/UI/IBase.h"
#include "Items/Container.h"
//#include "Engone/Engine.h"

class Hotbar : public engone::IBase {
public:
	Hotbar(const std::string& name);

	bool MouseEvent(int mx, int my, int action, int button) override;

	void Update(float delta) override;
	void Render() override;

	Container* container = nullptr; // Reference to current container to be shown.
};