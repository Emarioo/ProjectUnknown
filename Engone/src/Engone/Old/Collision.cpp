#include "Engone/Collision.h"

#include "Engone/Rendering/Renderer.h"

namespace engone {
	float insideLine(glm::vec3& a, glm::vec3& b, glm::vec3& p) {
		glm::vec3 norm = { -(b.z - a.z), 0, (b.x - a.x) };
		return norm.x * (p.x - a.x) + norm.z * (p.z - a.z);
	}
	float leng2(glm::vec3 v) {
		return sqrtf(v.x * v.x + v.z * v.z);
	}
	float closestLength(glm::vec3 a, glm::vec3 b, glm::vec3 p) {
		glm::vec3 r = b - a;
		/*
			o = a + t*r
			(o-p) dot r = 0

			(p.x-a.x-t*r.x)*r.x+(p.z-a.z-t*r.z)*r.z = 0
			(p.x-a.x)*r.x+(p.z-a.z)*r.z = t*r.x*r.x+t*r.z*r.z
			((p.x-a.x)*r.x+(p.z-a.z)*r.z)/(r.x*r.x+r.z*r.z) = t
		*/
		float t = ((p.x - a.x) * r.x + (p.z - a.z) * r.z) / (r.x * r.x + r.z * r.z);

		glm::vec3 o = a + t * r;

		return leng2(p - o);
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
	bool TestComplexCollision(Collider* c1, Collider* c2) {
		bool colliding = false;
		float time = 0;

		Transform* t1 = c1->entity.getComponent<Transform>();
		Transform* t2 = c2->entity.getComponent<Transform>();
		Physics* p1 = c1->entity.getComponent<Physics>();
		Physics* p2 = c2->entity.getComponent<Physics>();

		if (c1->asset->colliderType == ColliderAsset::Type::Sphere &&
			c2->asset->colliderType == ColliderAsset::Type::Sphere) {

			float dist = glm::length(c1->asset->sphere.position - c2->asset->sphere.position);
			if (dist < c1->asset->sphere.radius + c2->asset->sphere.radius) {
				colliding = true;
				time = 1;
			}

		} else if (c1->asset->colliderType == ColliderAsset::Type::Cube &&
			c2->asset->colliderType == ColliderAsset::Type::Cube) {

			using namespace glm;

			struct line {
				int a, b;
			};
			struct plane {
				glm::vec3 o, n;
				int ind[4];
			};

			/*auto getLine = [&](int x, int y, int z, int x2, int y2, int z2) {

				line line = { (c2->matrix * translate(vec3(c2->scale.x / 2 * x, c2->scale.y / 2 * y, c2->scale.z / 2 * z)))[3],
					(c2->matrix * translate(vec3(c2->scale.x / 2 * x2, c2->scale.y / 2 * y2, c2->scale.z / 2 * z2)))[3]
				};
				return line;
			};*/
			auto getPlane = [&](int x, int y, int z) {
				plane plane = { (c2->matrix * translate(vec3(c2->scale.x / 2 * x, c2->scale.y / 2 * y, c2->scale.z / 2 * z)))[3],
				normalize((c2->matrix * translate(vec3(c2->scale.x / 2 * x, c2->scale.y / 2 * y, c2->scale.z / 2 * z)))[3] - c2->matrix[3]) };
				if (x == -1) {
					for (int i = 0; i < 4; i++) {
						plane.ind[i] = i * 2;
					}
				} else if (x == 1) {
					for (int i = 0; i < 4; i++) {
						plane.ind[i] = 1 + i * 2;
					}
				} else if (y == -1) {
					for (int i = 0; i < 4; i++) {
						plane.ind[i] = i + (i / 2) * 2;
					}
				} else if (y == 1) {
					for (int i = 0; i < 4; i++) {
						plane.ind[i] = 2 + i + (i / 2) * 2;
					}
				} else if (z == -1) {
					for (int i = 0; i < 4; i++) {
						plane.ind[i] = i;
					}
				} else if (z == 1) {
					for (int i = 0; i < 4; i++) {
						plane.ind[i] = 4 + i;
					}
				}
				return plane;
			};

			// points for collider 1
			vec3 points1[8];
			for (int i = 0; i < 8; i++) {
				points1[i] = (c1->matrix * translate(vec3(
					(i % 2 == 0 ? -1 : 1) * c1->scale.x / 2,
					((i / 2) % 2 == 0 ? -1 : 1) * c1->scale.y / 2,
					((i / 4) % 2 == 0 ? -1 : 1) * c1->scale.z / 2)))[3];
			}
			vec3 points2[8];
			for (int i = 0; i < 8; i++) {
				points2[i] = (c2->matrix * translate(vec3(
					(i % 2 == 0 ? -1 : 1) * c2->scale.x / 2,
					((i / 2) % 2 == 0 ? -1 : 1) * c2->scale.y / 2,
					((i / 4) % 2 == 0 ? -1 : 1) * c2->scale.z / 2)))[3];
			}
			/*
			000
			100
			010
			110
			001
			101
			011
			111
			*/

			//DrawCube(c1->matrix);

			// lines for cube
			line lines[12];
			lines[0] = { 0,1 };
			lines[1] = { 0,2 };
			lines[2] = { 1,3 };
			lines[3] = { 2,3 };

			lines[4] = { 0,4 };
			lines[5] = { 1,5 };
			lines[6] = { 2,6 };
			lines[7] = { 3,7 };

			lines[8] = { 4,5 };
			lines[9] = { 4,6 };
			lines[10] = { 5,7 };
			lines[11] = { 6,7 };

			/*for (int i = 0; i < 12;i++) {
				AddLine(points[lines[i].a],points[lines[i].b]);
			}*/

			/*
			lines[0] = getLine(0, 0, 0, 1, 0, 0);
			lines[1] = getLine(0, 0, 0, 0, 0, 1);
			lines[2] = getLine(1, 0, 1, 1, 0, 0);
			lines[3] = getLine(1, 0, 1, 0, 0, 1);

			lines[4] = getLine(0, 0, 0, 0, 1, 0);
			lines[5] = getLine(1, 0, 0, 1, 1, 0);
			lines[6] = getLine(0, 0, 1, 0, 1, 1);
			lines[7] = getLine(1, 0, 1, 1, 1, 1);

			lines[8] = getLine(0, 1, 0, 1, 1, 0);
			lines[9] = getLine(0, 1, 0, 0, 1, 1);
			lines[10] = getLine(1, 1, 1, 1, 1, 0);
			lines[11] = getLine(1, 1, 1, 0, 1, 1);*/

			auto drawPlane = [&](plane plane) {
				AddLine(points2[plane.ind[0]] + plane.n / 4.f, points2[plane.ind[1]] + plane.n / 4.f);
				AddLine(points2[plane.ind[0]] + plane.n / 4.f, points2[plane.ind[2]] + plane.n / 4.f);
				AddLine(points2[plane.ind[3]] + plane.n / 4.f, points2[plane.ind[1]] + plane.n / 4.f);
				AddLine(points2[plane.ind[3]] + plane.n / 4.f, points2[plane.ind[2]] + plane.n / 4.f);
			};

			// planes for collider 2
			plane planes[6];
			planes[0] = getPlane(-1, 0, 0);
			planes[1] = getPlane(1, 0, 0);
			planes[2] = getPlane(0, -1, 0);
			planes[3] = getPlane(0, 1, 0);
			planes[4] = getPlane(0, 0, -1);
			planes[5] = getPlane(0, 0, 1);

			for (plane& plane : planes) {
				vec3 A = points2[plane.ind[0]];
				vec3 B = points2[plane.ind[1]];
				vec3 C = points2[plane.ind[2]];
				vec3 D = points2[plane.ind[3]];

				drawPlane(plane);

				/*AddLine(A,B);
				AddLine(B,C);
				AddLine(C,D);
				AddLine(D,A);*/

				vec3 nA = normalize(A - plane.o);
				vec3 nB = normalize(B - plane.o);
				vec3 nC = normalize(C - plane.o);
				vec3 nD = normalize(D - plane.o);

				for (line& line : lines) {
					vec3 P = IntersectPlane(plane.o, plane.n, points1[line.a], points1[line.b]);

					float a = glm::dot(nA, P - B);
					float b = glm::dot(nB, P - C);
					float c = glm::dot(nC, P - D);
					float d = glm::dot(nD, P - A);
					log::out << a << " " << b << " " << c << " " << d << "\n";
					// if P is inside then calculate how much.
					if (a < 0 && b < 0 && c < 0 && d < 0) {
						log::out << "collision\n";
						return true;
					}
				}
				break;
			}

		}
		return false;
	}
	bool TestCollision(Collider* c1, Collider* c2) {
		bool colliding = false;
		float time = 0;

		Transform* t1 = c1->entity.getComponent<Transform>();
		Transform* t2 = c2->entity.getComponent<Transform>();
		Physics* p1 = c1->entity.getComponent<Physics>();
		Physics* p2 = c2->entity.getComponent<Physics>();

		if (c1->asset->colliderType == ColliderAsset::Type::Sphere &&
			c2->asset->colliderType == ColliderAsset::Type::Sphere) {

			float dist = glm::length(c1->asset->sphere.position - c2->asset->sphere.position);
			if (dist < c1->asset->sphere.radius + c2->asset->sphere.radius) {
				colliding = true;
				time = 1;
			}

		} else if (c1->asset->colliderType == ColliderAsset::Type::Cube &&
			c2->asset->colliderType == ColliderAsset::Type::Cube) {

			// get collision points
			glm::vec3 move = t1->position - p1->lastPosition;
			if (move.x == 0 && move.y == 0 && move.z == 0) {
				move = t2->position - p2->lastPosition;
				if (move.x == 0 && move.y == 0 && move.z == 0)
					return false;
				Collider* temp = c1;
				Transform* tempT = t1;
				Physics* tempP = p1;
				c1 = c2;
				t1 = t2;
				p1 = p2;
				c2 = temp;
				t2 = tempT;
				p2 = tempP;
			}


			glm::vec3 diff = c1->position - (t1->position - p1->lastPosition) - (c2->position - (t2->position - p2->lastPosition));
			glm::vec3 point1 = c1->position - c1->scale / 2.f;
			glm::vec3 point2 = c2->position - c2->scale / 2.f;

			point1.x += (diff.x < 0) ? c1->scale.x : 0;
			point1.y += (diff.y < 0) ? c1->scale.y : 0;
			point1.z += (diff.z < 0) ? c1->scale.z : 0;
			point2.x += (diff.x < 0) ? 0 : c2->scale.x;
			point2.y += (diff.y < 0) ? 0 : c2->scale.y;
			point2.z += (diff.z < 0) ? 0 : c2->scale.z;

			float tx = (point2.x - point1.x) / move.x;
			float ty = (point2.y - point1.y) / move.y;
			float tz = (point2.z - point1.z) / move.z;

			glm::vec3 s1 = c1->scale;
			glm::vec3 v1 = c1->position - s1 / 2.f;
			glm::vec3 s2 = c2->scale;
			glm::vec3 v2 = c2->position - s2 / 2.f;

			if (v1.x + s1.x > v2.x && v1.x < v2.x + s2.x &&
				v1.y + s1.y > v2.y && v1.y < v2.y + s2.y &&
				v1.z + s1.z > v2.z && v1.z < v2.z + s2.z) {

				if (abs(tx) < abs(ty) && abs(tx) < abs(tz)) {
					//log::out << "(" << point2.x << "-" << point1.x << ")" << "/" << move.x << " = " << tx << "\n";
					t1->position.x += tx * move.x;
					p1->velocity.x = 0;
					return true;
				} else if (abs(ty) < abs(tz)) {
					//log::out << "(" << point2.y << "-" << point1.y << ")" << "/" << move.y << " = " << ty << "\n";
					t1->position.y += ty * move.y;
					p1->velocity.y = 0;
					return true;
				} else {
					//log::out << "(" << point2.z << "-" << point1.z << ")" << "/" << move.z << " = " << tz<<"\n";
					t1->position.z += tz * move.z;
					p1->velocity.z = 0;
					return true;
				}
			}
		} else if ((c1->asset->colliderType == ColliderAsset::Type::Mesh &&
			c2->asset->colliderType == ColliderAsset::Type::Cube) ||
			(c1->asset->colliderType == ColliderAsset::Type::Cube &&
				c2->asset->colliderType == ColliderAsset::Type::Mesh)) {

			if (c2->asset->colliderType == ColliderAsset::Type::Mesh) {
				Collider* tmp = c1;
				c1 = c2;
				c2 = tmp;
				Transform* tmpt = t1;
				t1 = t2;
				t2 = tmpt;
				Physics* tmpp = p1;
				p1 = p2;
				p2 = tmpp;
			}

			glm::vec3 p = c2->position;
			p.y -= c2->scale.y / 2.f;

			std::vector<uint16_t>& tris = c1->asset->tris;
			std::vector<glm::vec3>& points = c1->asset->points;

			for (size_t i = 0; i < tris.size() / 3; ++i) {
				glm::vec3 a = points[tris[3 * i]] + c1->position;
				glm::vec3 b = points[tris[3 * i + 1]] + c1->position;
				glm::vec3 c = points[tris[3 * i + 2]] + c1->position;

				//AddLine(a,p);
				//AddLine(b,p);
				//AddLine(c,p);

				float inA = insideLine(a, b, p);
				if (inA > 0) continue;
				float inB = insideLine(b, c, p);
				if (inB > 0) continue;
				float inC = insideLine(c, a, p);
				if (inC > 0) continue;

				//log::out << inA <<" "<< inB <<" "<< inC << "\n";

				float p_bc = closestLength(b, c, p);
				float a_bc = closestLength(b, c, a);
				float p_ac = closestLength(a, c, p);
				float b_ac = closestLength(a, c, b);
				float p_ab = closestLength(a, b, p);
				float c_ab = closestLength(a, b, c);

				float la = p_bc / a_bc;
				float lb = p_ac / b_ac;
				float lc = p_ab / c_ab;

				//log::out << (la) <<" "<< (lb) <<" "<< (lc) << "\n";

				float y = a.y * (la)+b.y * (lb)+c.y * (lc);

				if (p.y < y && p.y + c2->scale.y>y) {
					t2->position.y = y;
					p2->velocity.y = 0;
					return true;
				} else {
					return false;
				}
			}
		}
		return false;
	}
	bool TestPlanes(PlaneCollider& a, PlaneCollider& b) {

		glm::vec3 a0 = a.getPoint(0, 0);
		glm::vec3 a1 = a.getPoint(0, 1);
		glm::vec3 a2 = a.getPoint(0, 2);

		glm::vec3 b0 = b.getPoint(3, 0);
		glm::vec3 b1 = b.getPoint(3, 1);

		glm::vec3 n = glm::cross(a0 - a2, a1 - a2);

		glm::vec3 point = IntersectPlane(a2,n,b0,b1);

		AddLine(a0,a1);
		AddLine(a1,a2);
		AddLine(a0,a2);

		//log::out << b0 << " " << b1 << "\n";
		AddLine(b0,b1);

		AddLine(point,point+glm::vec3(0,0.1,0));
		
		return false;
	}

	//-- some code for testing collision
	//ui::Box box = { 20,30,100,100,{1,1,1,1} };
	//ui::Draw(box);

	//glm::mat4 m1 = glm::translate(glm::vec3(1, 8, 0));
	//glm::vec3 s1(1, 1, 1);

	//glm::mat4 m2 = glm::translate(glm::vec3(1, 8, 0));
	//glm::vec3 s2(1.5, 1, 1);

	//PlaneCollider col1 = MakeCubeCollider(m1, s1);
	//PlaneCollider col2 = MakeCubeCollider(m2, s2);

	//bool boolean = TestPlanes(col1, col2);
}