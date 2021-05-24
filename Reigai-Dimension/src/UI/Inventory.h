#pragma once

#include "Engine/UI/ISystem.h"
#include "Inventory/Container.h"

class Inventory : public engine::ISystem {
public:
	Inventory(const std::string& name);
	/*
	Override of functions
	*/
	void MouseEvent(double mx, double my, int button, int action) override;
	void Update(float delta) override;
	void Render() override;

	Container* container; // Refrence to current inventory to be shown.
};