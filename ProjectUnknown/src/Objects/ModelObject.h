#pragma once

#include "Engone/Objects/GameObject.h"

/*
This is a Template Object or a customizable object.
*/
class ModelObject : public engine::GameObject {
private:
	float blending = 0;
	float speed = 1;

public:
	ModelObject(float x, float y, float z, const std::string& name);
	void Update(float delta) override;

};