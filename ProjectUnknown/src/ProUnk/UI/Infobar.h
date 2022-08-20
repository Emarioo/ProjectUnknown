#pragma once

#include "Engone/UI/IBase.h"
#include "Items/Container.h"

class Infobar : public engone::IBase {
public:
	Infobar(const std::string& name);

	void Update(float delta) override;
	void Render() override;
};