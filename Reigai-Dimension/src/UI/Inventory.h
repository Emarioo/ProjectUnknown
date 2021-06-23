#pragma once

#include "Engine/UI/IBase.h"
#include "Inventory/Container.h"

class Inventory : public engine::IBase {
public:
	Inventory(const std::string& name);
	
	bool active;
	bool IsActive();

	bool ClickEvent(int mx, int my, int button, int action) override;
	bool KeyEvent(int key, int action) override;
	
	void Update(float delta) override;
	void Render() override;

	Container* container=nullptr; // Reference to current container to be shown.
};