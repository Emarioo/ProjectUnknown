#include "gonpch.h"

#include "GameObject.h"

namespace engone {
	GameObject::GameObject(const std::string& name) : m_name(name) {

	}
	GameObject::GameObject(const std::string& name, float x, float y, float z) : m_name(name) {
		//physics.position = { x,y,z };
	}

	void GameObject::setName(std::string name) {
		this->m_name = name;
	}
	std::string& GameObject::getName() {
		return m_name;
	}
	/*void GameObject::SetPosition(glm::vec3 v) {
		physics.position = v;
	}
	glm::vec3 GameObject::GetPosition() {
		return physics.position;
	}*/
	//void GameObject::SetModel(ModelAsset* model) {
	//	m_model = model;
	//}
	//ModelAsset* GameObject::GetModel() {
	//	return m_model;
	//}
	/*void GameObject::Update(float delta) {}
	void GameObject::OnCollision(Collider& my, Collider& their) {}*/
}