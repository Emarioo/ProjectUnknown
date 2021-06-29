#pragma once

#include "Engine/UI/IBase.h"
#include "Items/Container.h"
#include "Engine/Engine.h"

class Infobar : public engine::IBase {
public:
	Infobar(const std::string& name);

	bool active;
	bool IsActive();

	//bool ClickEvent(int mx, int my, int button, int action) override;
	//bool KeyEvent(int key, int action) override;

	void Update(float delta) override;
	void Render() override;
};