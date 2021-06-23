#include "GameObject.h"

#include "glm/gtx/intersect.hpp"

namespace engine {

	glm::vec3 GameObject::GetPos() {
		return position;
	}
	glm::vec3 GameObject::GetRot() {
		return rotation;
	}
	void GameObject::SetPosition(float x, float y, float z) {
		position.x = x;
		position.y = y;
		position.z = z;
		if (collisionComponent.isActive) {
			Location pos;
			pos.Translate(position);
			pos.Rotate(rotation);
			collisionComponent.SetMatrix(pos.mat());
		}
	}
	void GameObject::SetPosition(glm::vec3 v) {
		position.x = v.x;
		position.y = v.y;
		position.z = v.z;
		if (collisionComponent.isActive) {
			Location pos;
			pos.Translate(position);
			pos.Rotate(rotation);
			collisionComponent.SetMatrix(pos.mat());
		}
	}
	/*
	Not supported with collision
	*/
	void GameObject::SetRotation(float x, float y, float z) {
		rotation.x = x;
		rotation.y = y;
		rotation.z = z;
		if (collisionComponent.isActive) {
			Location pos;
			pos.Translate(position);
			pos.Rotate(rotation);
			collisionComponent.SetMatrix(pos.mat());
		}
	}
	void GameObject::SetRotation(glm::vec3 v) {
		rotation.x = v.x;
		rotation.y = v.y;
		rotation.z = v.z;
		if (collisionComponent.isActive) {
			Location pos;
			pos.Translate(position);
			pos.Rotate(rotation);
			collisionComponent.SetMatrix(pos.mat());
		}
	}
	glm::vec3 IntersectPlane(glm::vec3 plane_p, glm::vec3 plane_n, glm::vec3 lineStart, glm::vec3 lineEnd) {
		//float plane_d = glm::dot(plane_n, plane_p);
		float ad = glm::dot(lineStart, plane_n);
		//float bd = glm::dot(lineEnd, plane_n);
		//float t = (plane_d - ad) / (bd - ad);
		//glm::vec3 lineStartToEnd = lineEnd - lineStart;
		//glm::vec3 lineToIntersect = lineStartToEnd * t;
		//glm::vec3 finish = lineStart + lineToIntersect;
		//return finish;
		return lineStart + (lineEnd - lineStart) * (glm::dot(plane_n, plane_p) - ad) / (glm::dot(lineEnd, plane_n) - ad);
	}
	/*
	Will cause errors if collisionComponent is inactive
	*/
	glm::vec3 GameObject::WillCollide(GameObject* o1, float delta) {
		CollisionComponent& c0 = collisionComponent;
		CollisionComponent& c1 = o1->collisionComponent;
		glm::vec3 out = velocity;

		int times = 0;
		while (times++ < 1) {
			if (times == 2) {
				c1 = collisionComponent;
				c0 = o1->collisionComponent;
				out *= -1;
			}
			bool hasHit = true;
			int limit = 0;
			while (hasHit && limit++ < 5) {
				hasHit = false;
				glm::vec3 newOut;
				float shortest = 9999;
				for (int i = 0; i < c1.coll->quad.size() / 4; i++) {

					// Get 4 points for quad
					glm::vec3 A = c1.points[c1.coll->quad[i * 4]];
					glm::vec3 B = c1.points[c1.coll->quad[i * 4 + 1]];
					glm::vec3 C = c1.points[c1.coll->quad[i * 4 + 2]];
					glm::vec3 D = c1.points[c1.coll->quad[i * 4 + 3]];

					// Front of quad
					glm::vec3 up = glm::normalize(glm::cross(B - A, C - A));

					// Normal of the edges
					glm::vec3 AB = glm::normalize(glm::cross(up, A - B));
					glm::vec3 BC = glm::normalize(glm::cross(up, B - C));
					glm::vec3 CD = glm::normalize(glm::cross(up, C - D));
					glm::vec3 DA = glm::normalize(glm::cross(up, D - A));

					for (int j = 0; j < c0.points.size(); j++) {
						// Starting Point
						glm::vec3 M = c0.points[j];
						// Ending Point
						glm::vec3 N = c0.points[j] + out * delta;

						// Which side of plane is point j before and after velocity
						float f0 = glm::dot(M - A, up);
						float f1 = glm::dot(N - A, up);

						// If d0 and d1 is same then no collision
						if (f0 >= 0 && f1 >= 0 || f0 < 0 && f1 < 0) {
							continue;
						}

						// Line's intersection point to quad plane
						glm::vec3 P = IntersectPlane(A, up, M, N);

						// Values for wether P is outside of quad plane. (negative inside, positive outside)
						float a = glm::dot(AB, P - B);
						float b = glm::dot(BC, P - C);
						float c = glm::dot(CD, P - D);
						float d = glm::dot(DA, P - A);

						// if P is inside then calculate how much.
						if (a < 0 && b < 0 && c < 0 && d < 0) {
							float l = glm::length(P - M);

							// take the shortest intersection point
							if (l < shortest) {
								shortest = l;
								hasHit = true;

								// Closest point to plane
								glm::vec3 U = IntersectPlane(A, up, M, M + up);
								float mn = glm::length(out * delta);
								float um = glm::length(U - M);

								// Point beyond U
								glm::vec3 V = up * (-mn * um / l);

								newOut = (out * delta - V) / delta;
							}
						}
					}
				}
				if (hasHit)
					out = newOut;
			}
		}
		return out * -1.f;
	}
	GameObject::GameObject() {

	}
	void GameObject::SetName(std::string name) {
		this->name = name;
	}
	std::string& GameObject::GetName() {
		return name;
	}
	void GameObject::Update(float delta) {

	}
	bool GameObject::IsClose(GameObject* o) {
		return glm::length(GetPos() - o->GetPos()) < proximity + o->proximity;
	}
	void GameObject::MetaUpdate(float delta) {
		/*
		for (int i = 0; i < metaData.meta.size(); i++) {
			MetaStrip* m = &metaData.meta[i];

			// NOTE: Might need a description for all of these?

			if (m->types[0] == Velocity) {
				if (!m->bools[0]) {
					if (m->types[1] == Pos) {
						int a = 0;
						for (int j = 0; j < 3; j++) {
							position[j] += m->floats[j] * delta;
							if (m->floats[j] == 0)
								a++;
						}
						hasVelocity = a != 3;
					} else if (m->types[1] == Rot) {
						int a = 0;
						for (int j = 0; j < 3; j++) {
							rotation[j] += m->floats[j] * delta;
							if (m->floats[j] == 0)
								a++;
						}
						hasVelocity = a != 3;
					}
				}
			} else if (m->types[0] == Polation) {
				if (position[m->types[2]] < m->floats[0]) {
					MetaStrip* ms = metaData.GetMeta(Velocity, m->types[1], None);
					if (ms != nullptr) {
						ms->floats[m->types[2]] = abs(m->floats[2]);
					}
				} else if (position[m->types[2]] > m->floats[1]) {
					MetaStrip* ms = metaData.GetMeta(Velocity, m->types[1], None);
					if (ms != nullptr) {
						ms->floats[m->types[2]] = -abs(m->floats[2]);
					}
				}
			}
		}*/
	}
}