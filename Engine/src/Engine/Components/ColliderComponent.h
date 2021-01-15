#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Rendering/BufferContainer.h"
#include "Rendering/Renderer.h"
#include "Data/CollData.h"
namespace engine {
	class Location {
	public:
		Location();
		glm::mat4 loc;
		void Translate(glm::vec3 v);
		void Translate(float x, float y, float z);
		void Rotate(glm::vec3 v);
		void Scale(glm::vec3 v);
		void Matrix(glm::mat4 m);
		glm::mat4 mat();
		glm::vec3 vec();
	};

	class ColliderComponent {
	public:
		ColliderComponent() {}
		/*
		If collision is activated. Not used in collision class
		*/
		bool isActive = false;

		CollData* coll = nullptr;
		void SetData(CollData* data);

		glm::mat4 matrix = glm::mat4(1);
		void SetMatrix(glm::mat4 mat);
		/*
		Real world points
		*/
		std::vector<glm::vec3> points;
		void UpdatePoints();

		void MakeCube(float x, float y, float z, float w, float h, float d);

		bool IsClose(ColliderComponent*);// Is object close?
	};
}