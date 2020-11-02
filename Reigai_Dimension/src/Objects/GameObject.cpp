#include "GameObject.h"

#include "glm/gtx/intersect.hpp"

Location::Location()
: loc(1){

}
void Location::Rotate(glm::vec3 v) {
	if(v.z!=0)
		loc *= glm::rotate(v.z, glm::vec3(0, 0, 1));
	if(v.x!=0)
		loc *= glm::rotate(v.x, glm::vec3(1, 0, 0));
	if(v.y!=0)
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
	return loc*glm::vec4(1);
}

Collision::Collision() {

}
/*
void Collision::SetHitbox(float f0, float f1, float f2, float f3, float f4, float f5) {
	isSolid = true;
	x = f0;
	y = f1;
	z = f2;
	w = f3;
	h = f4;
	d = f5;
}
float Collision::center(int a) {
	if(a==0)
		return pos->x + x;
	else if (a == 1)
		return pos->y + y;
	else if (a == 2)
		return pos->z + z;
}
float Collision::sideX(bool b) {
	return center(0) + w / 2*(b?1:-1);
}
/*
True for top
*//*
float Collision::sideY(bool b) {
	return center(1) + h / 2 * (b ? 1 : -1);
}
float Collision::sideZ(bool b) {
	return center(2)+d/2 * (b ? 1 : -1);
}
std::vector<GameObject*> Collision::IsCollidingL() {
	std::vector<GameObject*> out;
	if (objects == nullptr) {
		std::cout << "Collision.objects is a nullptr" << std::endl;
	} else {
		for (GameObject* o : *objects) {
			Collision* c = &o->collision;
			if (!c->isSolid)
				continue;
			if (sideX(1) > c->sideX(0) &&
				sideX(0) < c->sideX(1)) {
				if (sideY(1) > c->sideY(0) &&
					sideY(0) < c->sideY(1)) {
					if (sideZ(1) > c->sideZ(0) &&
						sideZ(0) < c->sideZ(1)) {
						out.push_back(o);
					}
				}
			}
		}
	}
	return out;
}*/
/*
GameObject* Collision::IsColliding() {
	if (objects == nullptr) {
		std::cout << "Collision.objects is a nullptr" << std::endl;
	} else {
		for (GameObject* o : *objects) {
			Collision* c = &o->collision;
			if (!c->isSolid)
				continue;
			if (pos->x + x + w / 2 > c->pos->x + c->x - c->w / 2 &&
				pos->x + x - w / 2 < c->pos->x + c->x + c->w / 2) {
				if (pos->y + y + h / 2 > c->pos->y + c->y - c->h / 2 &&
					pos->y + y - h / 2 < c->pos->y + c->y + c->h / 2) {
					if (pos->z + z + d / 2 > c->pos->z + c->z - c->d / 2 &&
						pos->z + z - d / 2 < c->pos->z + c->z + c->d / 2) {
						return o;
					}
				}
			}
		}
	}
	return nullptr;
}
*/
void Collision::MakeCube(float x,float y,float z,float w,float h,float d) {
	glm::vec3 O = glm::vec3(x, y, z);

	int ind = originPoints.size();
	// Removed O -> push_back(O+glm::
	originPoints.push_back(glm::vec3(-w,-h,-d)/2.f);
	originPoints.push_back(glm::vec3(w,-h,-d)/2.f);
	originPoints.push_back(glm::vec3(w,-h,d)/2.f);
	originPoints.push_back(glm::vec3(-w,-h,d)/2.f);

	originPoints.push_back(glm::vec3(-w,h,-d)/2.f);
	originPoints.push_back(glm::vec3(w,h,-d)/2.f);
	originPoints.push_back(glm::vec3(w,h,d)/2.f);
	originPoints.push_back(glm::vec3(-w,h,d)/2.f);

	quadIndex.push_back(ind + 0);
	quadIndex.push_back(ind + 1);
	quadIndex.push_back(ind + 2);
	quadIndex.push_back(ind + 3);

	quadIndex.push_back(ind + 0);
	quadIndex.push_back(ind + 1);
	quadIndex.push_back(ind + 5);
	quadIndex.push_back(ind + 4);

	quadIndex.push_back(ind + 1);
	quadIndex.push_back(ind + 2);
	quadIndex.push_back(ind + 6);
	quadIndex.push_back(ind + 5);
	
	quadIndex.push_back(ind + 2);
	quadIndex.push_back(ind + 3);
	quadIndex.push_back(ind + 7);
	quadIndex.push_back(ind + 6);
	
	quadIndex.push_back(ind + 3);
	quadIndex.push_back(ind + 0);
	quadIndex.push_back(ind + 4);
	quadIndex.push_back(ind + 7);
	
	quadIndex.push_back(ind + 4);
	quadIndex.push_back(ind + 5);
	quadIndex.push_back(ind + 6);
	quadIndex.push_back(ind + 7);
	
	float fl[]{
		originPoints.at(ind + 0).x,originPoints.at(ind + 0).y,originPoints.at(ind + 0).z,
		originPoints.at(ind + 1).x,originPoints.at(ind + 1).y,originPoints.at(ind + 1).z,
		originPoints.at(ind + 2).x,originPoints.at(ind + 2).y,originPoints.at(ind + 2).z,
		originPoints.at(ind + 3).x,originPoints.at(ind + 3).y,originPoints.at(ind + 3).z,
		originPoints.at(ind + 4).x,originPoints.at(ind + 4).y,originPoints.at(ind + 4).z,
		originPoints.at(ind + 5).x,originPoints.at(ind + 5).y,originPoints.at(ind + 5).z,
		originPoints.at(ind + 6).x,originPoints.at(ind + 6).y,originPoints.at(ind + 6).z,
		originPoints.at(ind + 7).x,originPoints.at(ind + 7).y,originPoints.at(ind + 7).z
		//,0,0,0
	};
	unsigned int in[]{
		0,1,
		1,2,
		2,3,
		3,0,
		0,4,
		1,5,
		2,6,
		3,7,
		4,5,
		5,6,
		6,7,
		7,4
		
		/*,0,8,
		1,8,
		2,8,
		3,8,
		4,8,
		5,8,
		6,8,
		7,8/**/
	};
	outline.type = 1;
	outline.Setup(false, fl, 3 * (8/*+1/**/), in, 2 * (12/*+8/**/));
	outline.SetAttrib(0, 3, 3, 0);
	//points = originPoints;
	UpdatePoints(glm::vec3(0, 0, 0));
}
void Collision::MakeFace(glm::vec3 p1,glm::vec3 p2,glm::vec3 p3,glm::vec3 p4) {
	int found[4]{-1,-1,-1,-1};
	for (int i = 0; i < originPoints.size(); i++) {
		if (originPoints.at(i) == p1) {
			found[0] = i;
		}
		if (originPoints.at(i) == p2) {
			found[1] = i;
		}
		if (originPoints.at(i) == p3) {
			found[2] = i;
		}
		if (originPoints.at(i) == p4) {
			found[3] = i;
		}
	}
	if (found[0]==-1) {
		found[0] = originPoints.size();
		originPoints.push_back(p1);
	}
	if (found[1]==-1) {
		found[1] = originPoints.size();
		originPoints.push_back(p2);
	}
	if (found[2]==-1) {
		found[2] = originPoints.size();
		originPoints.push_back(p3);
	}
	if (found[3]==-1) {
		found[3] = originPoints.size();
		originPoints.push_back(p4);
	}
	quadIndex.push_back(found[0]);
	quadIndex.push_back(found[1]);
	quadIndex.push_back(found[2]);
	quadIndex.push_back(found[3]);
	// Add new faces?
	float fl[]{
		p1.x,p1.y,p1.z,
		p2.x,p2.y,p2.z,
		p3.x,p3.y,p3.z,
		p4.x,p4.y,p4.z
		
		//,0,0,0,
	};
	unsigned int in[]{
		0,1,
		1,2,
		2,3,
		3,0
		/*
		,4,0,
		4,1,
		4,2,
		4,3/**/
	};
	outline.type = 1;
	outline.Setup(false, fl, 3 * (4/*+1/**/), in, 2 * (4/*+4/**/));
	outline.SetAttrib(0, 3, 3, 0);
	//points = originPoints;
	UpdatePoints(glm::vec3(0, 0, 0));
}
void Collision::UpdatePoints(glm::vec3 r) {
	points.clear();
	points = originPoints;
	/*
	for (glm::vec3 p : originPoints) {
		Location loc;
		loc.Translate(p);
		//loc.Rotate(r);
		points.push_back(loc.vec());
	}*/
}
void Collision::MakeSphere(float rad) {
	type = 1;
	radius = rad;
}
bool Collision::IsClose(Collision* c2) {
	return glm::length(*oPos-*c2->oPos)<radius+c2->radius;
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
	return lineStart+(lineEnd-lineStart)*(glm::dot(plane_n, plane_p) - ad) / (glm::dot(lineEnd, plane_n) - ad);
}
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
glm::vec3 Collision::collision(Collision* c2) {
	Collision* p1 = this;
	Collision* p2 = c2;

	glm::vec3 out(0, 0, 0);
	float shortest = 9999;

	glm::vec3 Q = *p1->oPos;// Q is pos for first object
	glm::vec3 O = *p2->oPos;// O is pos for second object

	for (int i = 0; i < p1->quadIndex.size() / 4; i++) {
		
		// Get 4 points for quad
		glm::vec3 A = Q + p1->points[p1->quadIndex[i * 4]];
		glm::vec3 B = Q + p1->points[p1->quadIndex[i * 4 + 1]];
		glm::vec3 C = Q + p1->points[p1->quadIndex[i * 4 + 2]];
		glm::vec3 D = Q + p1->points[p1->quadIndex[i * 4 + 3]];
		
		// Front of quad
		glm::vec3 up = glm::normalize(glm::cross(B - A, C - A));

		// Which side is O on
		float d0 = glm::dot(O - A, up);

		/*pr("d0 ");
		pf(d0);
		pe();*/

		// Normal of the edges
		glm::vec3 AB = glm::normalize(glm::cross(up, A - B));
		glm::vec3 BC = glm::normalize(glm::cross(up, B - C));
		glm::vec3 CD = glm::normalize(glm::cross(up, C - D));
		glm::vec3 DA = glm::normalize(glm::cross(up, D - A));
		
		for (int j = 0; j < p2->points.size(); j++) {
			// Which side is j point of second object on
			float d1 = glm::dot(O + p2->points[j] - A, up);

			/*pr(" d1 ");
			pf(d1);
			pe();*/

			// If d0 is on same side as d1 then no collision has happened.
			if (d0 > 0 && d1 > 0 || d0 < 0 && d1 < 0) {
				continue;
			}
			
			pr("A ");
			pv(O);
			pv(p2->points[j]);
			pv(A);
			pv(up);
		
			pe();

			// Line's intersection point to quad plane
			glm::vec3 P = IntersectPlane(A, up, O, O + p2->points[j]);
			
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
				float l = glm::length(P - O);
				// take the shortest intersection point
				if (l < shortest) {
					shortest = l;
					//pr("Up"); pv(up); pe();
					/*if (d1<0) {
						out = up * (O - P);
					} else {
						out = up * (P - (O + p2->points[j]));
					}*/
					out = up * (P - (O + p2->points[j]));
				}
			}
		}
	}
	return out;
}
void Collision::Draw() {
	Location body;
	body.Translate(*oPos);
	//body.Rotate(*oRot);
	ObjectTransform(body.mat());
	ObjectColor(1, 1, 1, 1);
	outline.Draw();
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
	THIS METHOD CAN CAUSE COMPLICATIONS DEPENDING ON WHICH POINT IS USED 
	*/
	for (int i = 0; i < c1->quadIndex.size() / 4; i++) {

		// Get 4 points for quad
		glm::vec3 A = O + c1->points[c1->quadIndex[i * 4]];
		glm::vec3 B = O + c1->points[c1->quadIndex[i * 4 + 1]];
		glm::vec3 C = O + c1->points[c1->quadIndex[i * 4 + 2]];
		glm::vec3 D = O + c1->points[c1->quadIndex[i * 4 + 3]];

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

			pr("D ");
			pv(Q+c0->points[j]);
			pv(Q + c0->points[j]+v0*delta);
			pf(d0);
			pf(d1);
			pe();

			// If d0 and d1 is same then no collision
			if (d0 >= 0 && d1 >= 0 || d0 < 0 && d1 < 0) {
				continue;
			}
			
			pr("Diff! ");
			/*pv(O);
			pv(p2->points[j]);
			pv(A);
			pv(up);
			*/
			pe();
			
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
				float l = glm::length(P - Q);
				// take the shortest intersection point
				if (l < shortest) {
					shortest = l;
					//pr("Up"); pv(up); pe();
					/*if (d1<0) {
						out = up * (O - P);
					} else {
						out = up * (P - (O + p2->points[j]));
					}*/
					pr("Special!");
					pe();
					out = (P - Q + c0->points[j])/ delta;
				}
			}
		}
	}
	return out;
}
GameObject::GameObject() {
	isSolid = true;
	collision.oPos = &position;
	collision.oRot = &rotation;
}
GameObject::GameObject(float x,float y,float z,std::string mesh) {
	isSolid = true;
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