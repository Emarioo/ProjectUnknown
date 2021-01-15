#pragma once

//#include "Components/AnimationComponent.h"
//#include "Components/BoneComponent.h"
#include "Components/MetaComponent.h"
#include "Components/ColliderComponent.h"
#include "Components/RenderComponent.h"

#include "Managers/DataManager.h"

namespace engine {
	class GameObject {
	public:
		GameObject();
		glm::vec3 position;// Pointer to meta strip?
		glm::vec3 rotation;
		glm::vec3 scale;
		//ColliderComponent collider;
		//MaterialType
		/*
		Remember to use delta
		*/
		RenderComponent renderComponent;

		glm::vec3 velocity;
		std::string name;
		void SetName(std::string name);
		std::string& GetName();

		int weight = 1;

		float proximity = 40;

		virtual void Update(float delta);
		virtual void PreComponents();
		virtual std::vector<ColliderComponent*> GetColliders();

		glm::vec3 GetPos();
		glm::vec3 GetRot();
		void SetPosition(float x, float y, float z);
		void SetPosition(glm::vec3 v);
		void SetRotation(float x, float y, float z);
		void SetRotationA(float x, float y, float z);
		void SetRotation(glm::vec3 v);
		/*
		return is equal to this.velocity if no collision.
		*/
		glm::vec3 WillCollide(GameObject* o1, float delta);

		void MetaUpdate(float delta);
		MetaComponent metaData;
		bool hasVelocity = false;
		bool renderHitbox = false;
		bool renderMesh = true;

		bool IsClose(GameObject*);
	};
}