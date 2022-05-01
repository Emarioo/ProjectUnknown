
#include "Engone.h"

#include "EventHandler.h"
#include "Handlers/AssetHandler.h"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

namespace engone {

	static const char* guiShaderSource = {
#include "Shaders/gui.glsl"
	};
	static const char* objectSource = {
#include "Shaders/object.glsl"
	};
	static const char* armatureSource = {
#include "Shaders/armature.glsl"
	};
	static const char* collisionSource = {
#include "Shaders/collision.glsl"
	};

	static std::function<void(double)> update = nullptr;
	static std::function<void(double)> render = nullptr;

#define INSTANCE_LIMIT 1000
	
	VertexBuffer instanceBuffer;
	FrameBuffer depthBuffer;
	//glm::mat4 lightProjection;
	FrameBuffer& GetDepthBuffer() {
		return depthBuffer;
	}

	/*static glm::mat4 projMatrix;
	static glm::mat4 viewMatrix;
	static float fov, zNear, zFar;*/

	static int lastMouseX = -1, lastMouseY = -1;
	static double cameraSensitivity = 0.1;

	EventType FirstPerson(Event& e) {
		if (lastMouseX != -1) {
			if (GetWindow()->isCursorLocked() && GetCamera() != nullptr) {
				float pi = glm::pi<float>();
				GetCamera()->rotation.y -= (e.mx - lastMouseX) * (pi / 360) * cameraSensitivity;
				GetCamera()->rotation.x -= (e.my - lastMouseY) * (pi / 360) * cameraSensitivity;
				if (GetCamera()->rotation.x > pi /2) {
					GetCamera()->rotation.x = pi/2;
				}
				if (GetCamera()->rotation.x < -pi /2) {
					GetCamera()->rotation.x = -pi /2;
				}
			}
		}
		lastMouseX = e.mx;
		lastMouseY = e.my;
		return EventType::None;
	}
	EventType DrawOnResize(Event& e) {
		//glViewport(0, 0, Width(), Height());
		//glClearColor(1, 0, 0, 1);
		//glClear(GL_COLOR_BUFFER_BIT);

		//UpdateEngine(1 / 40.f);
		update(1 / 40.f);
		//RenderEngine(0);
		render(0);

		glfwSwapBuffers(GetWindow()->glfw());

		return EventType::None;
	}

	void InitEngone(EngoneHint hints) {
		SetEngoneHints(hints);
		// gl functions in game loop function
		/*glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);*/
		//glEnable(GL_FRAMEBUFFER_SRGB);

		//if (hints & EngoneHint::UI){
			AddAsset<Shader>("gui", new Shader(guiShaderSource));
			//InitGui();
			//InitUIPipeline();
			//InitRenderer(EngoneHint::UI);
		//}
		//if (hints & EngoneHint::Game3D) {
			AddAsset<Shader>("object", new Shader(objectSource));
			AddAsset<Shader>("armature", new Shader(armatureSource));
			AddAsset<Shader>("collision", new Shader(collisionSource));
			instanceBuffer.setData(100 * 16, nullptr);
			InitRenderer(EngoneHint::Game3D);
			AddListener(new Listener(EventType::Move, 9998, FirstPerson));
			depthBuffer.init();
			/*float near_plane = 1.f, far_plane = 20.5f;
			lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);

			fov = 90.f;
			zNear = 0.1f;
			zFar = 400.f;*/
			//SetProjection(GetWidth() / GetHeight());
		//}
		//if (hints & EngoneHint::Network) {
			
		//}
		//if (hints & EngoneHint::Sound) {

		//}

		AddListener(new Listener(EventType::Resize, 9999, DrawOnResize));
	}
	void UninitEngone() {

	}

	/*void SetProjection(float ratio) {
		projMatrix = glm::perspective(fov, ratio, zNear, zFar);
	}
	void UpdateViewMatrix(double lag) {
		viewMatrix = glm::inverse(
			glm::translate(glm::mat4(1.0f), GetCamera()->position) *
			glm::rotate(GetCamera()->rotation.y, glm::vec3(0, 1, 0)) *
			glm::rotate(GetCamera()->rotation.x, glm::vec3(1, 0, 0))
		);
	}
	void UpdateProjection(Shader* shader) {
		if (shader != nullptr)
			shader->setMat4("uProj", projMatrix * viewMatrix);
	}*/
	static float insideLine(glm::vec3& a, glm::vec3& b, glm::vec3& p) {
		glm::vec3 norm = { -(b.z - a.z), 0, (b.x - a.x) };
		return norm.x * (p.x - a.x) + norm.z * (p.z - a.z);
	}
	static float leng2(glm::vec3 v) {
		return sqrtf(v.x * v.x + v.z * v.z);
	}
	static float closestLength(glm::vec3 a, glm::vec3 b, glm::vec3 p) {
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
				normalize((c2->matrix * translate(vec3(c2->scale.x / 2 * x, c2->scale.y / 2 * y, c2->scale.z / 2 * z)))[3] - c2->matrix[3])};
				if (x == -1) {
					for (int i = 0; i < 4;i++) {
						plane.ind[i] = i*2;
					}
				} else if (x == 1) {
					for (int i = 0; i < 4; i++) {
						plane.ind[i] = 1+i * 2;
					}
				} else if (y == -1) {
					for (int i = 0; i < 4; i++) {
						plane.ind[i] = i+(i/2)*2;
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
			lines[0] = {0,1};
			lines[1] = {0,2};
			lines[2] = {1,3};
			lines[3] = {2,3};

			lines[4] = {0,4};
			lines[5] = {1,5};
			lines[6] = {2,6};
			lines[7] = {3,7};

			lines[8] = {4,5};
			lines[9] = {4,6};
			lines[10] = {5,7};
			lines[11] = {6,7};

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
				AddLine(points2[plane.ind[0]] + plane.n/4.f, points2[plane.ind[1]] + plane.n/4.f);
				AddLine(points2[plane.ind[0]] + plane.n/4.f, points2[plane.ind[2]] + plane.n/4.f);
				AddLine(points2[plane.ind[3]] + plane.n/4.f, points2[plane.ind[1]] + plane.n/4.f);
				AddLine(points2[plane.ind[3]] + plane.n/4.f, points2[plane.ind[2]] + plane.n/4.f);
			};

			// planes for collider 2
			plane planes[6];
			planes[0] = getPlane(-1,0,0);
			planes[1] = getPlane(1,0,0);
			planes[2] = getPlane(0,-1,0);
			planes[3] = getPlane(0,1,0);
			planes[4] = getPlane(0,0,-1);
			planes[5] = getPlane(0,0,1);

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

				vec3 nA = normalize(A-plane.o);
				vec3 nB = normalize(B-plane.o);
				vec3 nC = normalize(C-plane.o);
				vec3 nD = normalize(D-plane.o);

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
			
			
			glm::vec3 diff = c1->position-(t1->position-p1->lastPosition) - (c2->position-(t2->position-p2->lastPosition));
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
			c2->asset->colliderType == ColliderAsset::Type::Cube)||
			(c1->asset->colliderType == ColliderAsset::Type::Cube &&
				c2->asset->colliderType == ColliderAsset::Type::Mesh)) {

			if (c2->asset->colliderType==ColliderAsset::Type::Mesh) {
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
			p.y-=c2->scale.y / 2.f;

			std::vector<uint16_t>& tris = c1->asset->tris;
			std::vector<glm::vec3>& points = c1->asset->points;

			for (int i = 0; i < tris.size()/3;i++) {
				glm::vec3 a = points[tris[3*i]]+ c1->position;
				glm::vec3 b = points[tris[3*i+1]]+ c1->position;
				glm::vec3 c = points[tris[3*i+2]]+ c1->position;

				//AddLine(a,p);
				//AddLine(b,p);
				//AddLine(c,p);

				float inA = insideLine(a,b,p);
				if (inA>0) continue;
				float inB = insideLine(b,c,p);
				if (inB>0) continue;
				float inC = insideLine(c,a,p);
				if (inC>0) continue;

				//log::out << inA <<" "<< inB <<" "<< inC << "\n";

				float p_bc = closestLength(b, c, p);
				float a_bc = closestLength(b, c, a);
				float p_ac = closestLength(a, c, p);
				float b_ac = closestLength(a, c, b);
				float p_ab = closestLength(a, b, p);
				float c_ab = closestLength(a, b, c);

				float la = p_bc/a_bc;
				float lb = p_ac/b_ac;
				float lc = p_ab/c_ab;

				//log::out << (la) <<" "<< (lb) <<" "<< (lc) << "\n";
				
				float y = a.y*(la)+b.y*(lb)+c.y*(lc);
				
				if (p.y < y&&p.y+c2->scale.y>y) {
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

	void UpdateObjects(float delta) 
	{
		std::vector<Collider> colliders;
	
		// Space partitioning?
		
		EntityIterator iterator = GetEntityIterator(Transform::ID | Physics::ID);
		while (iterator) {
			Transform* transform = iterator.get<Transform>();
			Physics* physics = iterator.get<Physics>();
			physics->lastPosition = transform->position;
		}
		
		RunUpdateSystems(delta);

		//-- Movement and physics
		//EntityIterator iterator = GetEntityIterator(ComponentEnum::Transform|ComponentEnum::Physics);
		while (iterator) {
			Transform* transform = iterator.get<Transform>();
			Physics* physics = iterator.get<Physics>();
			//ModelRenderer* renderer = iterator.get<ModelRenderer>();
			//Scriptable* scriptable = iterator.get<Scriptable>();
			/*
			if (scriptable)
				scriptable->entity->Update(delta);
			*/
			if (physics->movable) {
				physics->velocity.y += physics->gravity * delta;
				transform->position += physics->velocity * delta;
			}
		}
		iterator = GetEntityIterator(Transform::ID | Physics::ID | Collision::ID);
		while (iterator) {
			Transform* transform = iterator.get<Transform>();
			Physics* physics = iterator.get<Physics>();
			Collision* collision = iterator.get<Collision>();

			ColliderAsset* asset = collision->asset;

			glm::mat4 matrix = glm::translate(transform->position);

			if (asset->colliderType == ColliderAsset::Type::Cube) {
				matrix = glm::scale(matrix, asset->cube.scale);
			}

			colliders.push_back({ asset, iterator.getEntity() });
			glm::vec3 skew;
			glm::quat rotation;
			glm::vec4 persp;
			glm::decompose(matrix, colliders.back().scale, rotation, colliders.back().position, skew, persp);

			if (asset->colliderType == ColliderAsset::Type::Sphere) {
				colliders.back().position += asset->sphere.position;
			} else if (asset->colliderType == ColliderAsset::Type::Cube) {
				colliders.back().position += asset->cube.position;
			}
		}
		iterator = GetEntityIterator(Transform::ID | Physics::ID | ModelRenderer::ID);
		while(iterator){
			Transform* transform = iterator.get<Transform>();
			Physics* physics = iterator.get<Physics>();
			ModelRenderer* renderer = iterator.get<ModelRenderer>();

			if (renderer->asset) {
				ModelAsset* model = renderer->asset;

				glm::mat4 modelMatrix = glm::translate(transform->position)
					* glm::rotate(transform->rotation.x, glm::vec3(1, 0, 0))
					* glm::rotate(transform->rotation.z, glm::vec3(0, 0, 1))
					* glm::rotate(transform->rotation.y, glm::vec3(0, 1, 0))
					* glm::scale(transform->scale);

				// Get individual transforms
				std::vector<glm::mat4> transforms;
				Animator* animator = iterator.get<Animator>();
				if(animator)
					model->getParentTransforms(animator, transforms);

				for (int j = 0; j < model->instances.size(); j++) {
					AssetInstance& instance = model->instances[j];

					if (instance.asset->type == AssetType::Collider) {
						ColliderAsset* asset = instance.asset->cast<ColliderAsset>();

						glm::mat4 matrix;
						if(animator)
							matrix = modelMatrix * transforms[j] * instance.localMat;
						else
							matrix = modelMatrix * instance.localMat;

						/*if (asset->colliderType==ColliderAsset::Type::Cube) {
							matrix = glm::scale(matrix, asset->cube.scale);
						}*/

						if (asset->colliderType == ColliderAsset::Type::Sphere) {
							matrix *= glm::translate(asset->sphere.position);
							//colliders.back().position += asset->sphere.position;
						} else if (asset->colliderType == ColliderAsset::Type::Cube) {
							matrix *= glm::translate(asset->cube.position);
							//colliders.back().position += asset->cube.position;
						}

						colliders.push_back({ asset, iterator.getEntity() });
						colliders.back().matrix = matrix;

						glm::vec3 skew;
						glm::quat rotation;
						glm::vec4 persp;
						glm::decompose(matrix, colliders.back().scale, rotation, colliders.back().position, skew, persp);

						/*if (asset->colliderType == ColliderAsset::Type::Sphere) {
							colliders.back().position += asset->sphere.position;
						} else if (asset->colliderType == ColliderAsset::Type::Cube) {
							colliders.back().position += asset->cube.position;
						}*/
					}
				}
			}

		}

		//-- Collision
		while (colliders.size()>1) {
			Collider& c1 = colliders[0];
			for (int i = 1; i < colliders.size();i++) {
				Collider& c2 = colliders[i];

				if (glm::length(c1.position - c2.position) < c1.asset->furthest+c2.asset->furthest) {
					
					bool yes = TestComplexCollision(&c1,&c2);
					if (yes) {
						RunCollisionSystems(c1, c2);
					}
				}
			}
			// remove collider when done with calculations
			colliders.erase(colliders.begin());
		}
	}
	void RenderRawObjects(Shader* shader, double lag) {
		//for (GameObject* o : GetObjects()) {
			/*
			if (o->renderComponent.model != nullptr) {
				for (int i = 0; i < o->renderComponent.model->meshes.size(); i++) {
					//std::cout << " okay2\n";
					Mesh* mesh = o->renderComponent.model->meshes[i];
					if (mesh != nullptr) {
						//std::cout << o->GetName()<<" okay3\n";
						shader->SetMatrix("uTransform", o->renderComponent.matrix);
						/*  *
						glm::translate(o->velocity * (float)lag)* // special
							o->renderComponent.model->matrices[i]
						/*
						SetTransform(o->renderComponent.matrix*
							glm::translate(o->velocity*(float)lag)* // special
							o->renderComponent.model->matrices[i]);
						
						mesh->Draw();
					}
				}
			}
			*/
		//}
	}
	void RenderObjects(double lag) {
		// Apply lag transformation on objects?

		EnableDepth();
		UpdateViewMatrix(lag);

		std::unordered_map<MeshAsset*, std::vector<glm::mat4>> normalObjects;
		//std::unordered_map<MeshAsset*, std::vector<glm::mat4>> bonedObjects; Not supported

		EntityIterator meshIterator = GetEntityIterator(Transform::ID | MeshRenderer::ID);
		while (meshIterator) {
			MeshRenderer* renderer = meshIterator.get<MeshRenderer>();
			if (renderer->asset && renderer->visible) {
				Transform* transform = meshIterator.get<Transform>();
				glm::mat4 modelMatrix = glm::translate(transform->position)
					* glm::rotate(transform->rotation.x, glm::vec3(1, 0, 0))
					* glm::rotate(transform->rotation.z, glm::vec3(0, 0, 1))
					* glm::rotate(transform->rotation.y, glm::vec3(0, 1, 0))
					* glm::scale(transform->scale);

				MeshAsset* asset = renderer->asset;

				if (asset->meshType == MeshAsset::MeshType::Normal) {
					if (normalObjects.count(asset) > 0) {
						normalObjects[asset].push_back(modelMatrix);
					} else {
						normalObjects[asset] = std::vector<glm::mat4>();
						normalObjects[asset].push_back(modelMatrix);
					}
				}
			}
		}
		EntityIterator modelIterator = GetEntityIterator(Transform::ID | ModelRenderer::ID);
		while (modelIterator) {
			ModelRenderer* renderer = modelIterator.get<ModelRenderer>();

			if (renderer->asset && renderer->visible) {
				ModelAsset* model = renderer->asset;

				Transform* transform = modelIterator.get<Transform>();
				glm::mat4 modelMatrix = glm::translate(transform->position)
					* glm::rotate(transform->rotation.x, glm::vec3(1, 0, 0))
					* glm::rotate(transform->rotation.z, glm::vec3(0, 0, 1))
					* glm::rotate(transform->rotation.y, glm::vec3(0, 1, 0))
					* glm::scale(transform->scale);

				Animator* animator = modelIterator.get<Animator>();

				// Get individual transforms
				std::vector<glm::mat4> transforms;
				if (animator)
					model->getParentTransforms(animator, transforms);

				/*
				if ((transform->position.x-camPos.x)* (transform->position.x - camPos.x)
					+(transform->position.y - camPos.y)*(transform->position.y - camPos.y)+
					(transform->position.z - camPos.z)*(transform->position.z - camPos.z)>200*200) {
					continue;
				}
				*/

				// Draw instances
				for (int i = 0; i < model->instances.size(); i++) {
					AssetInstance& instance = model->instances[i];
					//log::out << " " << instance.asset->filePath<< " " << (int)instance.asset->type << " "<< (int)asset->meshType << "\n";
					if (instance.asset->type == AssetType::Mesh) {
						MeshAsset* asset = instance.asset->cast<MeshAsset>();

						if (asset->meshType == MeshAsset::MeshType::Normal) {
							glm::mat4 out;

							if (animator)
								out = modelMatrix * transforms[i] * instance.localMat;
							else
								out = modelMatrix * instance.localMat;

							if (normalObjects.count(asset) > 0) {
								normalObjects[asset].push_back(out);
							} else {
								normalObjects[asset] = std::vector<glm::mat4>();
								normalObjects[asset].push_back(out);
							}
						}
					}
				}
			}
		}

		Shader* shader = GetAsset<Shader>("object");
		if (!shader->error) {
			shader->bind();
			UpdateProjection(shader);
			shader->setVec3("uCamera", GetCamera()->position + GetCamera()->velocity * (float)lag);
			glm::vec3 camPos = GetCamera()->position;

			BindLights(shader, { 0,0,0 });
			shader->setMat4("uTransform", glm::mat4(0));
			for (auto [asset, vector] : normalObjects) {
				//if (asset->meshType == MeshAsset::MeshType::Normal) {
					for (int j = 0; j < asset->materials.size(); j++) {// Maximum of 4 materials
						asset->materials[j]->bind(shader, j);
					}
				//}
				int renderAmount = 0;
				while (renderAmount<vector.size()) {
					int amount = vector.size()-renderAmount;
					if (amount>INSTANCE_LIMIT) {
						amount = INSTANCE_LIMIT;
					}
					instanceBuffer.setData(amount * 16, (float*)(vector.data()+renderAmount));
					
					asset->vertexArray.selectBuffer(3, &instanceBuffer);

					asset->vertexArray.draw(&asset->indexBuffer, amount);
					renderAmount += amount;
				}
			}
			/*
			EntityIterator iterator = GetEntityIterator(Transform::ID |ModelRenderer::ID);
			while (false) {
				ModelRenderer* renderer = iterator.get<ModelRenderer>();
				
				if (renderer->asset&&renderer->visible) {
					ModelAsset* model = renderer->asset;
					
					Transform* transform = iterator.get<Transform>();
					Animator* animator = iterator.get<Animator>();
					
					BindLights(shader, transform->position);

					// Get individual transforms
					std::vector<glm::mat4> transforms;
					if (animator)
						model->GetParentTransforms(animator, transforms);
					
					if ((transform->position.x-camPos.x)* (transform->position.x - camPos.x)
						+(transform->position.y - camPos.y)*(transform->position.y - camPos.y)+
						(transform->position.z - camPos.z)*(transform->position.z - camPos.z)>200*200) {
						continue;
					}

					// Draw instances
					for (int i = 0; i < model->instances.size(); i++) {
						AssetInstance& instance = model->instances[i];
						//log::out << " " << instance.asset->filePath<< " " << (int)instance.asset->type << " "<< (int)asset->meshType << "\n";
						if (instance.asset->type == AssetType::Mesh) {
							MeshAsset* asset = instance.asset->cast<MeshAsset>();
							if (asset->meshType == MeshAsset::MeshType::Normal) {
								for (int j = 0; j < asset->materials.size() && j < 4; j++)// Maximum of 4 materials
								{
									asset->materials[j]->Bind(shader, j);
								}
								if(animator)
									shader->SetMatrix("uTransform", glm::translate(transform->position) * transforms[i] * instance.localMat);
								else
									shader->SetMatrix("uTransform", glm::translate(transform->position) * instance.localMat);
								asset->vertexArray.draw(&asset->indexBuffer);
							}
						}
					}
				}
			}
			*/
		}

		shader = GetAsset<Shader>("armature");
		if (!shader->error) {
			shader->bind();
			UpdateProjection(shader);
			shader->setVec3("uCamera", GetCamera()->position + GetCamera()->velocity * (float)lag);

			//EntityIterator iterator = GetEntityIterator(ComponentEnum::Transform | ComponentEnum::ModelRenderer);
			while (modelIterator) {
				ModelRenderer* renderer = modelIterator.get<ModelRenderer>();

				if (renderer->asset && renderer->visible) {
					ModelAsset* model = renderer->asset;
				
					Transform* transform = modelIterator.get<Transform>();
					glm::mat4 modelMatrix = glm::translate(glm::mat4(1), transform->position)
						* glm::rotate(transform->rotation.x, glm::vec3(1, 0, 0))
						* glm::rotate(transform->rotation.z, glm::vec3(0, 0, 1))
						* glm::rotate(transform->rotation.y, glm::vec3(0, 1, 0))
						* glm::scale(transform->scale);

					Animator* animator = modelIterator.get<Animator>();

					BindLights(shader, transform->position);

					// Get individual transforms
					std::vector<glm::mat4> transforms;
					if (animator)
						model->getParentTransforms(animator, transforms);

					//-- Draw instances
					AssetInstance* armatureInstance=nullptr;
					ArmatureAsset* armatureAsset=nullptr;
					for (int i = 0; i < model->instances.size(); i++) {
						AssetInstance& instance = model->instances[i];

						if (instance.asset->type == AssetType::Armature) {
							armatureInstance = &instance;
							armatureAsset = instance.asset->cast<ArmatureAsset>();
						} else if (instance.asset->type == AssetType::Mesh) {
							if (!armatureInstance)
								continue;

							MeshAsset* meshAsset = instance.asset->cast<MeshAsset>();
							if (meshAsset->meshType == MeshAsset::MeshType::Boned) {
								if (instance.parent == -1)
									continue;

								/*if (animator->enabledAnimations[0].asset) {
									log::out << animator->enabledAnimations->frame<<"\n";
								}*/

								std::vector<glm::mat4> boneTransforms;
								if (animator)
									model->getArmatureTransforms(animator, boneTransforms, transforms[i], armatureInstance, armatureAsset);

								for (int j = 0; j < boneTransforms.size(); j++) {
									shader->setMat4("uBoneTransforms[" + std::to_string(j) + "]", boneTransforms[j] * instance.localMat);
								}

								for (int j = 0; j < meshAsset->materials.size(); j++){// Maximum of 4 materials
									meshAsset->materials[j]->bind(shader, j);
								}
								if (animator) {
									shader->setMat4("uTransform", modelMatrix * transforms[i]);
								} else {
									shader->setMat4("uTransform", modelMatrix);
								}
								meshAsset->vertexArray.draw(&meshAsset->indexBuffer);
							}
						}
					}
				}
			}
		}
		shader = GetAsset<Shader>("collision");
		if (shader != nullptr) {
			shader->bind();
			UpdateProjection(shader);
			glLineWidth(1.f);
			shader->setVec3("uColor", { 0.05,0.9,0.1 });

			EntityIterator modelIterator = GetEntityIterator(ComponentEnum::Transform | ComponentEnum::ModelRenderer | ComponentEnum::Physics);
			while (modelIterator) {
				ModelRenderer* renderer = modelIterator.get<ModelRenderer>();
				Physics* physics = modelIterator.get<Physics>();

				if (renderer->asset && physics->renderCollision) {
					ModelAsset* model = renderer->asset;

					Transform* transform = modelIterator.get<Transform>();
					glm::mat4 modelMatrix = glm::translate(glm::mat4(1), transform->position)
						* glm::rotate(transform->rotation.x, glm::vec3(1, 0, 0))
						* glm::rotate(transform->rotation.z, glm::vec3(0, 0, 1))
						* glm::rotate(transform->rotation.y, glm::vec3(0, 1, 0))
						* glm::scale(transform->scale);

					Animator* animator = modelIterator.get<Animator>();

					// Get individual transforms
					std::vector<glm::mat4> transforms;
					if (animator)
						model->getParentTransforms(animator, transforms);

					// Draw instances
					for (int i = 0; i < model->instances.size(); i++) {
						AssetInstance& instance = model->instances[i];
						//log::out << " " << instance.asset->filePath<< " " << (int)instance.asset->type << " "<< (int)asset->meshType << "\n";
						if (instance.asset->type == AssetType::Collider) {
							ColliderAsset* asset = instance.asset->cast<ColliderAsset>();
							glm::vec3 offset = glm::vec3(0);
							if (asset->colliderType == ColliderAsset::Type::Sphere) {
								offset = asset->sphere.position;
							} else if (asset->colliderType == ColliderAsset::Type::Cube) {
								offset = asset->cube.position;
							}
							glm::mat4 out;
							if (animator)
								out = modelMatrix * glm::translate(offset) * transforms[i] * instance.localMat;
							else
								out = modelMatrix * glm::translate(offset) * instance.localMat;

							if (asset->colliderType == ColliderAsset::Type::Sphere) {
								//DrawSphere(asset->sphere.radius);

							} else if (asset->colliderType == ColliderAsset::Type::Cube) {
								DrawNetCube(out, asset->cube.scale);
								//DrawCube(asset->cube.scale.x, asset->cube.scale.y, asset->cube.scale.z);
							} else if (asset->colliderType == ColliderAsset::Type::Mesh) {
								DrawBegin();
								for (int i = 0; i < asset->points.size(); i++) {
									AddVertex(asset->points[i].x, asset->points[i].y, asset->points[i].z);
								}
								auto& tris = asset->tris;
								for (int i = 0; i < asset->tris.size() / 3; i++) {
									AddIndex(tris[i * 3], tris[i * 3 + 1]);
									AddIndex(tris[i * 3 + 1], tris[i * 3 + 2]);
									AddIndex(tris[i * 3 + 2], tris[i * 3]);
								}
								DrawBuffer();
							}
						}
					}
				}
			}

			//debug
			shader->setVec3("uColor", { 0.7,0.1,0.1 });
			shader->setMat4("uTransform", glm::mat4(1));

			DrawLines();
			//ClearLines();
		}
		/*
		Mesh* lightCube = GetMeshAsset("LightCube");
		if (lightCube!=nullptr) {
			if (BindShader(ShaderLight)) {
				PassProjection();
				//bug::out < "ea" < GetLights().size()< bug::end;
				for (Light* l : GetLights()) {
					//if (l->lightType != LightType::Direction) {
						//bug::out < "rend light" < bug::end;
						GetShader(ShaderCurrent)->SetVec3("uLightColor", l->diffuse);
						PassTransform(glm::translate(l->position));
						lightCube->Draw();
					//}
				}
			}
		}
		*/
		/*
		if (BindShader(ShaderType::Terrain)) {
			SetProjection();
			Dimension* dim = GetDimension();
			if (dim != nullptr) {
				BindTexture(0,"blank");
				for (Chunk c : dim->loadedChunks) {
					Location base;
					base.Translate(glm::vec3(c.x * (dim->chunkSize), 0, c.z * (dim->chunkSize)));
					PassTransform(base.mat());
					c.con.Draw();
				}
			}
		}*/
	}
	void RenderEngine(double lag) {
		glViewport(0, 0, GetWidth(), GetHeight());

		EnableDepth();

		if (GetWindow()->hasFocus()) {

			//glClearColor(0.05f, 0.08f, 0.08f, 1);
			glClearColor(0.15f, 0.18f, 0.18f, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glCullFace(GL_BACK);
			if (GetEngoneHints() & EngoneHint::Game3D) {
				// All this should be automated and customizable in the engine.
				// Shadow stuff
				glm::mat4 lightView = glm::lookAt({ -2,4,-1 }, glm::vec3(0), { 0,1,0 });
				/*
				Shader* depth = GetAsset<Shader>("depth");
				if (depth != nullptr) {
					depth->Bind();
					glCullFace(GL_BACK);
					depth->SetMatrix("uLightMatrix", GetLightProj() * lightView);
					glViewport(0, 0, 1024, 1024);
					GetDepthBuffer().Bind();
					glClear(GL_DEPTH_BUFFER_BIT);
					RenderRawObjects(depth, lag);
					GetDepthBuffer().Unbind();
				}
				*/
				Shader* objectShader = GetAsset<Shader>("object");
				if (objectShader != nullptr) {
					objectShader->bind();

					objectShader->setInt("shadow_map", 0);
					objectShader->setMat4("uLightSpaceMatrix", GetLightProj() * lightView);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, GetDepthBuffer().texture);

					MeshAsset* asset = GetAsset<MeshAsset>("Axis/Cube.031");

					for (int j = 0; j < asset->materials.size(); j++) {// Maximum of 4 materials
						asset->materials[j]->bind(objectShader, j);
					}
					objectShader->setMat4("uTransform", glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
					asset->vertexArray.draw(&asset->indexBuffer);

					RenderObjects(lag);

					//objectShader->SetMatrix("uTransform", glm::mat4(1));
					//testBuffer.Draw();
				}/*
				if (IsKeyDown(GLFW_KEY_K)) {
					//std::cout << "yes\n";
					SwitchBlendDepth(true);
					Shader* experiment = GetAsset<Shader>("experiment");
					if (experiment != nullptr) {
						experiment->Bind();
						experiment->SetInt("uTexture", 0);
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, GetDepthBuffer().texture);
						cont.Draw();
					}
				}*/
				RenderRenderer();
				//-- Debug
				EnableBlend();
				Shader* gui = GetAsset<Shader>("gui");
				gui->bind();
				gui->setVec2("uPos", { 0,0 });
				gui->setVec2("uWindow", { GetWidth(), GetHeight() });
				gui->setVec2("uSize", { 1,1 });
				gui->setVec4("uColor", 1, 1, 1, 1);
				gui->setInt("uColorMode", 1);
				DrawString(GetAsset<Font>("consolas"), std::to_string(GetCamera()->position.x) + " " + std::to_string(GetCamera()->position.y) + " " + std::to_string(GetCamera()->position.z), false, 50, 300, 50, -1);
				//DrawString(GetAsset<Font>("consolas"), "Chicken ma swwet sdo", false, 50, 300, 50, -1);
			}
		}

		if (GetEngoneHints() & EngoneHint::UI) {
			//RenderElements();
			RenderUIPipeline();
		}
	}
	void UpdateEngine(double delta) {
		//if (CheckState(GameState::RenderGame)) {
			//if (HasFocus() && !CheckState(GameState::Paused)) {
		if (GetEngoneHints() & EngoneHint::Game3D) {
			UpdateObjects(delta);
		}
			//}
		//}
		UpdateTimers(delta);
		//if (CheckState(GameState::RenderGui)) {
			//UpdateElements(delta);
		//}
	}
	static std::vector<Delayed> timers;
	void AddTimer(float time, std::function<void()> func) {
		timers.push_back({ time,func });
	}
	void UpdateTimers(float delta) {
		for (int i = 0; i < timers.size(); i++) { // Will this cause problems? Removing an element in a for loop
			timers[i].time -= delta;
			if (timers[i].time < 0) {
				timers[i].func();
				timers.erase(timers.begin() + i);
				i--;
			}
		}
	}

	static std::vector<Light*> lights;
	void AddLight(Light* l)
	{
		lights.push_back(l);
	}
	void RemoveLight(Light* l)
	{
		for (int i = 0; i < lights.size(); i++) {
			if (lights[i] == l) {
				lights.erase(lights.begin() + i);
				break;
			}
		}
	}
	void BindLights(Shader* shader, glm::vec3 objectPos)
	{
		if (shader != nullptr) {
			// List setup
			const int N_POINTLIGHTS = 4;
			const int N_SPOTLIGHTS = 2;

			DirLight* dir = nullptr;
			PointLight* points[N_POINTLIGHTS];
			float pDist[N_POINTLIGHTS];
			for (int i = 0; i < N_POINTLIGHTS; i++) {
				points[i] = nullptr;
				pDist[i] = 9999;
			}

			SpotLight* spots[N_SPOTLIGHTS];
			float sDist[N_SPOTLIGHTS];
			for (int i = 0; i < N_SPOTLIGHTS; i++) {
				spots[i] = nullptr;
				sDist[i] = 9999;
			}
			glm::ivec3 lightCount(0);

			// Grab the closest lights to the object
			for (int i = 0; i < lights.size(); i++) {
				Light* light = lights[i];
				if (light->lightType == LightType::Direction) {
					dir = reinterpret_cast<DirLight*>(light);
				}
				else if (light->lightType == LightType::Point) {
					PointLight* l = reinterpret_cast<PointLight*>(light);
					float distance = glm::length(l->position - objectPos);
					for (int i = 0; i < N_POINTLIGHTS; i++) {
						if (points[i] == nullptr) {
							points[i] = l;
							pDist[i] = distance;
							break;
						}
						else {
							if (distance < pDist[i]) {
								PointLight* pTemp = points[i];
								float dTemp = pDist[i];
								points[i] = l;
								pDist[i] = distance;
							}
						}
					}
				}
				else if (light->lightType == LightType::Spot) {
					SpotLight* l = reinterpret_cast<SpotLight*>(light);
					float distance = glm::length(l->position - objectPos);
					for (int i = 0; i < N_SPOTLIGHTS; i++) {
						if (spots[i] == nullptr) {
							spots[i] = l;
							sDist[i] = distance;
							break;
						}
						else {
							if (distance < sDist[i]) {
								SpotLight* lTemp = spots[i];
								float dTemp = sDist[i];
								spots[i] = l;
								sDist[i] = distance;
							}
						}
					}
				}
			}

			// Pass light to shader
			if (dir != nullptr) {
				dir->bind(shader);
				lightCount.x++;
			}

			for (int i = 0; i < N_POINTLIGHTS; i++) {
				if (points[i] != nullptr) {
					points[i]->bind(shader, i);
					lightCount.y++;
				}
				else
					break;
			}
			for (int i = 0; i < N_SPOTLIGHTS; i++) {
				if (spots[i] != nullptr) {
					spots[i]->bind(shader, i);
					lightCount.z++;
				}
				else
					break;
			}
			shader->setIVec3("uLightCount", lightCount);
		}
	}
	std::vector<Light*>& GetLights()
	{
		return lights;
	}
	void Start(std::function<void(double)> inUpdate, std::function<void(double)> inRender, double fps) {
		if (inUpdate == nullptr || inRender == nullptr) {
			std::cout << "Engone.cpp:Start() - update or render is nullptr" << "\n";
			return;
		}
		update = inUpdate;
		render = inRender;

		/*
		// Loop
		int lastTime = GetTime();
		int lastDTime = lastTime;
		int FPS = 0;
		int tickSpeed = 0;
		float delta;

		while (RenderRunning()) {
			int now = GetTime();
			if (now - lastTime > 1000) {
				lastTime = now;
				SetWindowTitle(("Project Unknown  " + std::to_string(FPS) + " fps").c_str());
				FPS = 0;
				//std::cout << "Running" << std::endl;
			}

			delta = (now - lastDTime) / 1000.f;
			lastDTime = now;
			tickSpeed++;
			if (tickSpeed == 1) {
				tickSpeed = 0;
				FPS++;
				//Update(1.f/60);//delta);
				update(delta);

				render();
			}

			glfwSwapBuffers(GetWindow());
			glfwPollEvents();
		}*/

		double previous = GetAppTime();
		double lag = 0.0;
		double MS_PER_UPDATE = 1. / fps;
		double lastSecond = previous;
		int FPS = 0;
		while (GetWindow()->isRunning()) {
			double current = GetAppTime();
			double elapsed = current - previous;
			//std::cout << elapsed << std::endl;
			previous = current;
			lag += elapsed;

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glDepthFunc(GL_LESS);
			glEnable(GL_CULL_FACE);

			/*glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/

			//while (lag >= MS_PER_UPDATE) {
				//UpdateEngine(MS_PER_UPDATE);
			UpdateEngine(MS_PER_UPDATE);
			update(MS_PER_UPDATE);
			//	lag -= MS_PER_UPDATE;
			//	FPS++;
			//	if (current - lastSecond > 1) {
			//		//std::cout << (current - lastSecond) << std::endl;
			//		lastSecond = current;
			//		//SetWindowTitle(("Project Unknown  " + std::to_string(FPS) + " fps").c_str());
			//		FPS = 0;
			//	}
			//}

			render(lag);
			RenderEngine(lag);

			// reset
			while (PollChar());
			ResetEvents();

			glfwSwapBuffers(GetWindow()->glfw());
			glfwPollEvents();
		}

		// remove buffer and stuff or not?
		glfwTerminate();
	}
}