#pragma once

#include "GameObject.h"

class Cube : public GameObject {
public:
	Cube(float x,float y,float z,float w,float h,float d);
	Cube(float x,float y,float z,float w,float h,float d,float r, float g, float b, float a);
	void PreComponents() override;
	void Update(float delta) override;
	void GenMesh(float r,float g,float b,float a);
	//void MakeCube(float x,float y,float z,float w,float h,float d);
	float w, h, d;
	BufferContainer container;
};