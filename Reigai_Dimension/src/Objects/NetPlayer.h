#pragma once

#include "GameObject.h"

class NetPlayer : public GameObject {
public:
	NetPlayer(float x, float y, float z);
	void Draw() override;
	void Update(float delta) override;

	Animation* idle;

};