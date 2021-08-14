#pragma once

#include "../Rendering/BufferContainer.h"
#include "../Handlers/AssetHandler.h"

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
	class CollisionComponent {
	public:
		CollisionComponent() {}
		/*
		If collision is activated. Not used in collision class
		*/
		bool isActive = false;

		Collider* coll = nullptr;
		void SetCollider(const std::string& name);

		glm::mat4 matrix = glm::mat4(1);
		void SetMatrix(glm::mat4 mat);
		/*
		Real world points
		*/
		std::vector<glm::vec3> points;
		void UpdatePoints();

		//void MakeCube(float x, float y, float z, float w, float h, float d);

		// Is object close?
		bool IsClose(CollisionComponent*);
	};
}