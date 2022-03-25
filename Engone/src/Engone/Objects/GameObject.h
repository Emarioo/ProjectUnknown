#pragma once

//#include "../Components/Animator.h"
//#include "../Components/Physics.h"
//#include "../Components/RenderComponent.h"

#include "../Handlers/AssetHandler.h"

namespace engone {
	class GameObject {
	private:
		std::string m_name;

		//ModelAsset* m_model;

	public:
		GameObject(const std::string& name);
		GameObject(const std::string& name, float x,float y,float z);
		
		/*
		This can be altered in the game loop to apply custom transformation, rotation and scaling.
		*/
		glm::mat4 m_matrix=glm::mat4(1);

		//Animator animator;
		//RenderComponent renderComponent;
		//PhysicsG physics;

		void setName(std::string name);
		std::string& getName();
		
		//void SetPosition(glm::vec3 v);
		//glm::vec3 GetPosition();

		//void SetModel(ModelAsset* model);
		//ModelAsset* GetModel();
		
		//virtual void Update(float delta);
		//virtual void OnCollision(Collider& my, Collider& their);
	};
}