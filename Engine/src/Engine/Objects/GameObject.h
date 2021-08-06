#pragma once

#include "../Components/MetaComponent.h"
#include "../Components/CollisionComponent.h"
#include "../Components/RenderComponent.h"

#include "../Handlers/AssetHandler.h"

namespace engine {
	class GameObject {
	public:
		GameObject();
		GameObject(float x,float y,float z);
		glm::vec3 position;// Pointer to meta strip?
		glm::vec3 velocity;
		glm::vec3 rotation;
		glm::vec3 scale;

		//MaterialType
		/*
		Remember to use delta
		*/

		RenderComponent renderComponent;
		MetaComponent metaComponent;
		CollisionComponent collisionComponent;

		std::string name;
		void SetName(std::string name);
		std::string& GetName();

		int weight = 1;

		float proximity = 40;

		virtual void Update(float delta);

		glm::vec3 GetPos();
		glm::vec3 GetRot();
		void SetPosition(float x, float y, float z);
		void SetPosition(glm::vec3 v);
		void SetRotation(float x, float y, float z);
		void SetRotation(glm::vec3 v);
		/*
		return is equal to this.velocity if no collision.
		*/
		glm::vec3 WillCollide(GameObject* o1, float delta);

		void MetaUpdate(float delta);
		bool renderHitbox = false;
		bool renderMesh = true;

		bool IsClose(GameObject*);
	};
}