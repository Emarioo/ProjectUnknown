#pragma once

#include "Engine/UI/IBase.h"
#include "Items/Container.h"
#include "Engine/Engine.h"

class Infobar : public engine::IBase {
public:
	Infobar(const std::string& name);

	void Update(float delta) override;
	void Render() override;
};