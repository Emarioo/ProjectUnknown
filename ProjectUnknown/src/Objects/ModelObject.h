#pragma once

#include "Engine/Objects/GameObject.h"

/*
This is a Template Object or a customizable object.
*/
class ModelObject : public engine::GameObject {
private:
	float blending = 0;
	float speed = 1.7;

public:
	ModelObject(float x, float y, float z);
	void Update(float delta) override;

};