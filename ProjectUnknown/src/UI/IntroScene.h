#pragma once

#include "Engone/UI/IBase.h"

class IntroScene : public engone::IBase {
public:
	IntroScene(const std::string& name);

	void Update(float delta) override;
	void Render() override;
};