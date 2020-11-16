#include "Collision.h"

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
/*
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
		7,8/*
	};
	outline.type = 1;
	outline.Setup(true, fl, 3 * (8/*+1/*), in, 2 * (12/*+8/*));
	outline.SetAttrib(0, 3, 3, 0);
	//points = originPoints;
	UpdatePoints(glm::vec3(0, 0, 0));
}
void Collision::MakeFace(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4) {
	int found[4]{ -1,-1,-1,-1 };
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
	if (found[0] == -1) {
		found[0] = originPoints.size();
		originPoints.push_back(p1);
	}
	if (found[1] == -1) {
		found[1] = originPoints.size();
		originPoints.push_back(p2);
	}
	if (found[2] == -1) {
		found[2] = originPoints.size();
		originPoints.push_back(p3);
	}
	if (found[3] == -1) {
		found[3] = originPoints.size();
		originPoints.push_back(p4);
	}
	quadIndex.push_back(found[3]);
	quadIndex.push_back(found[2]);
	quadIndex.push_back(found[1]);
	quadIndex.push_back(found[0]);
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
		4,3/*
	};
	outline.type = 1;
	outline.Setup(true, fl, 3 * (4/*+1/*), in, 2 * (4/*+4/*));
	outline.SetAttrib(0, 3, 3, 0);
	//points = originPoints;
	UpdatePoints(glm::vec3(0, 0, 0));
}*/
void Collision::UpdatePoints(glm::vec3 r) {
	points.clear();
	//points = originPoints;

	for (glm::vec3 p : originPoints) {
		Location loc;
		//loc.Rotate(r);
		loc.Translate(p);

		//glm::vec4 at = glm::vec4(p.x,p.y,p.z,1)*glm::rotate(r.y,glm::vec3(0,1,0));
		//at *= glm::rotate(r.y, glm::vec3(0, 1, 0));
		/*pv(p);
		pv(glm::vec3(at.x, at.y, at.z));
		pe();*/

		points.push_back(loc.vec());//glm::vec3(at.x,at.y,at.z));
	}
	float fl[]{
		points.at(0).x,points.at(0).y,points.at(0).z,
		points.at(1).x,points.at(1).y,points.at(1).z,
		points.at(2).x,points.at(2).y,points.at(2).z,
		points.at(3).x,points.at(3).y,points.at(3).z,
		points.at(4).x,points.at(4).y,points.at(4).z,
		points.at(5).x,points.at(5).y,points.at(5).z,
		points.at(6).x,points.at(6).y,points.at(6).z,
		points.at(7).x,points.at(7).y,points.at(7).z
		//,0,0,0
	};
	outline.SubVB(0, 3 * 8, fl);
}/*
void Collision::MakeSphere(float rad) {
	type = 1;
	radius = rad;
}*/
bool Collision::IsClose(Collision* c2) {
	return glm::length(*oPos - *c2->oPos) < radius + c2->radius;
}
/*
void pr(std::string s) {
	std::cout << s << " ";
}
void pv(glm::vec3 v) {
	std::cout << "[" << v.x << " " << v.y << " " << v.z << "] ";
}
void pv(glm::vec2 v) {
	std::cout << "[" << v.x << " " << v.y << "] ";
}
void pf(float v) {
	std::cout << v << " ";
}
void pe() {
	std::cout << std::endl;
}*/
/*
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
		pe();

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
			pe();

			// If d0 is on same side as d1 then no collision has happened.
			if (d0 > 0 && d1 > 0 || d0 < 0 && d1 < 0) {
				continue;
			}
			/*
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
					}
					out = up * (P - (O + p2->points[j]));
				}
			}
		}
	}
	return out;
}*/
void Collision::Draw() {
	
	Location body;
	body.Translate(*oPos);
	//body.Rotate(*oRot);
	ObjectTransform(body.mat());
	ObjectColor(1, 1, 1, 1);
	outline.Draw();
}

void Collision::MakeCube(float x, float y, float z, float w, float h, float d) {
	glm::vec3 O = glm::vec3(x, y, z);

	originPoints.push_back(O + glm::vec3(-w, -h, -d) / 2.f);
	originPoints.push_back(O + glm::vec3(w, -h, -d) / 2.f);
	originPoints.push_back(O + glm::vec3(w, -h, d) / 2.f);
	originPoints.push_back(O + glm::vec3(-w, -h, d) / 2.f);

	originPoints.push_back(O + glm::vec3(-w, h, -d) / 2.f);
	originPoints.push_back(O + glm::vec3(w, h, -d) / 2.f);
	originPoints.push_back(O + glm::vec3(w, h, d) / 2.f);
	originPoints.push_back(O + glm::vec3(-w, h, d) / 2.f);

	quadIndex.push_back(0);
	quadIndex.push_back(1);
	quadIndex.push_back(2);
	quadIndex.push_back(3);

	quadIndex.push_back(0);
	quadIndex.push_back(1);
	quadIndex.push_back(5);
	quadIndex.push_back(4);

	quadIndex.push_back(1);
	quadIndex.push_back(2);
	quadIndex.push_back(6);
	quadIndex.push_back(5);

	quadIndex.push_back(2);
	quadIndex.push_back(3);
	quadIndex.push_back(7);
	quadIndex.push_back(6);

	quadIndex.push_back(3);
	quadIndex.push_back(0);
	quadIndex.push_back(4);
	quadIndex.push_back(7);

	quadIndex.push_back(4);
	quadIndex.push_back(5);
	quadIndex.push_back(6);
	quadIndex.push_back(7);

	float fl[]{
		originPoints.at(0).x,originPoints.at(0).y,originPoints.at(0).z,
		originPoints.at(1).x,originPoints.at(1).y,originPoints.at(1).z,
		originPoints.at(2).x,originPoints.at(2).y,originPoints.at(2).z,
		originPoints.at(3).x,originPoints.at(3).y,originPoints.at(3).z,
		originPoints.at(4).x,originPoints.at(4).y,originPoints.at(4).z,
		originPoints.at(5).x,originPoints.at(5).y,originPoints.at(5).z,
		originPoints.at(6).x,originPoints.at(6).y,originPoints.at(6).z,
		originPoints.at(7).x,originPoints.at(7).y,originPoints.at(7).z
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
	outline.Setup(true, fl, 3 * (8/*+1/**/), in, 2 * (12/*+8/**/));
	outline.SetAttrib(0, 3, 3, 0);
	//points = originPoints;
	UpdatePoints(glm::vec3(0, 0, 0));
}