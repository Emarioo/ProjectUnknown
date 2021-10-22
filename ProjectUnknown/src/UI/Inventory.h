#pragma once

#include "Engone/UI/IBase.h"
#include "Items/Container.h"

class Inventory : public engone::IBase {
public:
	Inventory(const std::string& name);

	bool MouseEvent(int mx, int my, int action, int button) override;
	bool KeyEvent(int key, int action) override;
	
	void Update(float delta) override;
	void Render() override;

	Container* container=nullptr; // Reference to current container to be shown.
};