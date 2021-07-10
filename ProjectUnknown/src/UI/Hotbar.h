#pragma once

#include "Engine/UI/IBase.h"
#include "Items/Container.h"
#include "Engine/Engine.h"

class Hotbar : public engine::IBase {
public:
	Hotbar(const std::string& name);

	bool MouseEvent(int mx, int my, int action, int button) override;

	void Update(float delta) override;
	void Render() override;

	Container* container = nullptr; // Reference to current container to be shown.
};