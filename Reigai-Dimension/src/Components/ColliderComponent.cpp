#include "ColliderComponent.h"

Location::Location()
	: loc(1) {

}
void Location::Rotate(glm::vec3 v) {
	if (v.z != 0)
		loc *= glm::rotate(v.z, glm::vec3(0, 0, 1));
	if (v.x != 0)
		loc *= glm::rotate(v.x, glm::vec3(1, 0, 0));
	if (v.y != 0)
		loc *= glm::rotate(v.y, glm::vec3(0, 1, 0));
}
void Location::Translate(glm::vec3 v) {
	loc *= glm::translate(v);
}
void Location::Scale(glm::vec3 v) {
	loc *= glm::scale(v);
}
glm::mat4 Location::mat() {
	return loc;
}
glm::vec3 Location::vec() {
	return loc[3];
}
void ColliderComponent::UpdatePoints() {
	points.clear();
	if (coll != nullptr) {
		for (glm::vec3 p : coll->points) {
			Location loc;
			loc.Translate(p);
			loc.Translate(matrix[3]);
			points.push_back(loc.vec());
		}
	}
}

bool ColliderComponent::IsClose(ColliderComponent* c2) {
	//if(coll!=nullptr&&c2->coll!=nullptr)
		//return glm::length(*oPos - *c2->oPos) < d->furthestPoint + d2->furthestPoint;
	//return false;

	return true;
}
void ColliderComponent::SetColl(CollData* c) {
	isActive = true;
	coll = c;
	UpdatePoints();
}
void ColliderComponent::SetMatrix(glm::mat4 m) {
	matrix = m;
	UpdatePoints();
}
/*
Run collider.SetColl("a_name_which_is_not_used")
before calling MakeCube(...)
*/
void ColliderComponent::MakeCube(float x, float y, float z, float w, float h, float d) {
	glm::vec3 O = glm::vec3(x, y, z);

	if (coll == nullptr)
		return;

	coll->furthestPoint = sqrt(pow(x+w/2, 2) + pow(y+h/2, 2) + pow(z+d/2, 2));

	coll->points.push_back(O + glm::vec3(-w, -h, -d) / 2.f);
	coll->points.push_back(O + glm::vec3(w, -h, -d) / 2.f);
	coll->points.push_back(O + glm::vec3(w, -h, d) / 2.f);
	coll->points.push_back(O + glm::vec3(-w, -h, d) / 2.f);
	
	coll->points.push_back(O + glm::vec3(-w, h, -d) / 2.f);
	coll->points.push_back(O + glm::vec3(w, h, -d) / 2.f);
	coll->points.push_back(O + glm::vec3(w, h, d) / 2.f);
	coll->points.push_back(O + glm::vec3(-w, h, d) / 2.f);
	
	coll->quad.push_back(0);
	coll->quad.push_back(1);
	coll->quad.push_back(2);
	coll->quad.push_back(3);
	
	coll->quad.push_back(0);
	coll->quad.push_back(1);
	coll->quad.push_back(5);
	coll->quad.push_back(4);

	coll->quad.push_back(1);
	coll->quad.push_back(2);
	coll->quad.push_back(6);
	coll->quad.push_back(5);

	coll->quad.push_back(2);
	coll->quad.push_back(3);
	coll->quad.push_back(7);
	coll->quad.push_back(6);

	coll->quad.push_back(3);
	coll->quad.push_back(0);
	coll->quad.push_back(4);
	coll->quad.push_back(7);

	coll->quad.push_back(4);
	coll->quad.push_back(5);
	coll->quad.push_back(6);
	coll->quad.push_back(7);
	
	UpdatePoints();
}