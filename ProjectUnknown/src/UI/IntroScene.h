#pragma once

#include "Engone/UI/IBase.h"

class IntroScene : public engine::IBase {
public:
	IntroScene(const std::string& name);

	void Update(float delta) override;
	void Render() override;
};