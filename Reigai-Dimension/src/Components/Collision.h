#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Rendering/BufferContainer.h"
#include "Rendering/Renderer.h"

class Location {
public:
	Location();
	glm::mat4 loc;
	void Translate(glm::vec3 v);
	void Rotate(glm::vec3 v);
	void Scale(glm::vec3 v);
	glm::mat4 mat();
	glm::vec3 vec();
};

class Collision {
public:
	Collision() {}
	/*
	If collision is activated. Not used in collision class
	*/
	bool isActive = true;

	/*
	
	*/
	glm::vec3 *oPos = nullptr, *oRot=nullptr;
	
	/*
	If this collision uses a static collision data or customized.
	*/
	bool isUnique = false;
	std::string colData = "";

	std::vector<glm::vec3> originPoints;
	std::vector<int> quadIndex;// 4 ints is one quad

	std::vector<glm::vec3> points;
	void UpdatePoints(glm::vec3 r);

	void MakeCube(float x, float y, float z, float w, float h, float d);

	float radius = 0;

	bool IsClose(Collision*);// Is object close?

	BufferContainer outline;
	void Draw();
};