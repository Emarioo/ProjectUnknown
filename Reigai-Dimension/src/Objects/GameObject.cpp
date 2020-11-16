#include "GameObject.h"

#include "glm/gtx/intersect.hpp"

void pr(std::string s) {
	std::cout << s<<" ";
}
void pv(glm::vec3 v) {
	std::cout << "["<<v.x << " " << v.y << " " << v.z<<"] ";
}
void pv(glm::vec2 v) {
	std::cout << "[" << v.x << " " << v.y << "] ";
}
void pf(float v) {
	std::cout << v<<" ";
}
void pe() {
	std::cout << std::endl;
}

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
}
/*
Not supported with collision
*/
void GameObject::SetRotation(float x, float y, float z) {
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
	//collision.UpdatePoints(rotation);
}
void GameObject::SetRotationA(float x, float y, float z) {
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
}
void GameObject::SetRotation(glm::vec3 v) {
	rotation.x = v.x;
	rotation.y = v.y;
	rotation.z = v.z;
	//collision.UpdatePoints(rotation);
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

glm::vec3 GameObject::WillCollide(GameObject* o1,float delta) {
	GameObject* o0 = this;

	Collision* c0 = &o0->collision;
	Collision* c1 = &o1->collision;

	glm::vec3 v0 = o0->velocity;

	glm::vec3 out = v0;
	float shortest = 9999;

	glm::vec3 Q = *c0->oPos;// Q is pos for first object
	glm::vec3 O = *c1->oPos;// O is pos for second object
	/*
	THIS METHOD CAN CAUSE COMPLICATIONS DEPENDING ON WHICH POINT IS USED. What do I mean?
	*/
	//pr("--------");
	//pe();
	for (int i = 0; i < c1->data->quad.size() / 4; i++) {

		// Get 4 points for quad
		glm::vec3 A = O + c1->points[c1->data->quad[i * 4]];
		glm::vec3 B = O + c1->points[c1->data->quad[i * 4 + 1]];
		glm::vec3 C = O + c1->points[c1->data->quad[i * 4 + 2]];
		glm::vec3 D = O + c1->points[c1->data->quad[i * 4 + 3]];

		// Front of quad
		glm::vec3 up = glm::normalize(glm::cross(B - A, C - A));
	

		// Normal of the edges
		glm::vec3 AB = glm::normalize(glm::cross(up, A - B));
		glm::vec3 BC = glm::normalize(glm::cross(up, B - C));
		glm::vec3 CD = glm::normalize(glm::cross(up, C - D));
		glm::vec3 DA = glm::normalize(glm::cross(up, D - A));
	
		for (int j = 0; j < c0->points.size(); j++) {
			// Which side of plane is point j before and after velocity
			float d0 = glm::dot(Q + c0->points[j] - A, up);
			float d1 = glm::dot(Q + c0->points[j] + v0*delta - A, up);

			/*pr("D ");
			pv(Q+c0->points[j]);
			pv(Q + c0->points[j]+v0*delta);
			pf(d0);
			pf(d1);
			pe();*/

			// If d0 and d1 is same then no collision
			if (d0 >= 0 && d1 >= 0 || d0 < 0 && d1 < 0) {
				continue;
			}
			/*
			pr("V ");
			pv(v0 * delta);
			//pv(c1->points[j]);
			//pv(A);
			//pv(up);
			
			pe();
			*/
			// Line's intersection point to quad plane
			glm::vec3 P = IntersectPlane(A, up, Q + c0->points[j], Q + c0->points[j] + v0*delta);

			// Values for wether P is outside of quad plane. (negative inside, positive outside)
			float a = glm::dot(AB, P - B);
			float b = glm::dot(BC, P - C);
			float c = glm::dot(CD, P - D);
			float d = glm::dot(DA, P - A);
			/*
			pr("Dots ");
			pf(a);
			pf(b);
			pf(c);
			pf(d);
			pe();
			*/
			// if P is inside then calculate how much.
			if (a < 0 && b < 0 && c < 0 && d < 0) {
				float l = glm::length(P - Q - c0->points[j]);
				// take the shortest intersection point
				if (l < shortest) {
					shortest = l;

					glm::vec3 up2 = glm::normalize(C - A);
					glm::vec3 right = glm::normalize(B - A);

					//glm::vec3 axis = glm::vec3(1- abs(up.x), 1- abs(up.y), 1- abs(up.z));
					//out = axis*v0;
					//out = v0 * glm::vec3(abs(up2.x),abs(up2.y),abs(up2.z)) + v0 * glm::vec3(abs(right.x),abs(right.y),abs(right.z));
					//out = -(P + up * 0.00001f- Q - c0->points[j]) / delta +v0 - glm::vec3(abs(up.x), abs(up.y), abs(up.z)) * v0;
					
					//out = up * (P - Q - c0->points[j])/delta;
					//out = v0-v0*glm::vec3(abs(up.x),abs(up.y),abs(up.z)); // Without rotation
					out = v0 - up * glm::vec3(abs(v0.x), abs(v0.y), abs(v0.z));
					//out *= 100.f;
					// TODO: This is an expensive calculation. Is there a better way?
					// TODO: This does NOT snap to edge, this will cause noticable gaps at high velocities.
					
					//pv(P + up * 0.00001f - Q - c0->points[j]);
					
					//pf(i);
					//pf(j);
					//pf(shortest);
					//pv(Q + c0->points[j]);
					//pv(Q + c0->points[j] + out*delta);
					/*pv(up);
					pv(v0*delta);
					pv(out*delta);
					//pv(Q + c0->points[j] - P + up * 0.00001f);
					pe();*/
					
					/*
					pr("I");
					pf(l);
					pf(i);// plane
					pf(j);// point
					pe();*/
					//pv(v0 * delta);
					//pv(up);
					//pv(out*delta);
					//pe();
				}
			}
		}
	}
	return out;
}
GameObject::GameObject() {
	collision.oPos = &position;
	collision.oRot = &rotation;
}
GameObject::GameObject(float x,float y,float z,std::string mesh) {
	SetPosition(x, y, z);
	standard_mesh = mesh;
	collision.oPos = &position;
	collision.oRot = &rotation;
}
void GameObject::SetName(std::string name) {
	this->name = name;
}
std::string& GameObject::GetName() {
	return name;
}
void GameObject::Draw() {
	//std::cout << "Draw GameObject" << std::endl;
	
	if (!standard_mesh.empty()) {
		Location body;
		body.Translate(position);
		body.Rotate(rotation);
		
		DrawMesh(standard_mesh, body.mat());
	}
}
void GameObject::MetaUpdate(float delta) {
	for (int i = 0; i < metaData.meta.size(); i++) {
		MetaStrip* m = &metaData.meta[i];
		
		// NOTE: Might need a description for all of these?

		if (m->types[0] == Velocity) {
			if (!m->bools[0]) {
				if (m->types[1] == Pos) {
					int a = 0;
					for (int j = 0; j <3 ;j++) {
						position[j] += m->floats[j]*delta;
						if (m->floats[j] == 0)
							a++;
					}
					hasVelocity = a != 3;
				}else if (m->types[1] == Rot) {
					int a = 0;
					for (int j = 0; j < 3; j++) {
						rotation[j] += m->floats[j]*delta;
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
	}
}
void GameObject::Update(float delta) {
	
}