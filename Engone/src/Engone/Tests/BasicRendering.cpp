#include "Engone/Rendering/Buffer.h"
#include "Engone/AssetModule.h"
#include "Engone/Engone.h"
#include "Engone/ParticleModule.h"

namespace engone {
	// Do one test at a time. The tests share resources.
	namespace test {

		static const char* test2dGLSL = {
#include "Engone/Tests/test2d.glsl"
		};
		static const char* test3dGLSL = {
#include "Engone/Tests/test3d.glsl"
		};

		static VertexBuffer VBO;
		static IndexBuffer IBO;
		static VertexArray VAO;
		static Shader* shader2 = nullptr;
		static Shader* shader3 = nullptr;
		static bool init_test2d=false;
		static bool init_test3d=false;
		void Test2D(LoopInfo& info) {
			if (!shader2) {

				shader2 = ALLOC_NEW(Shader)(test2dGLSL);
				float fArray[]{
					0,0,
					1,0,
					1,1
				};
				uint32_t iArray[]{
					0,1,2,
				};
				VBO.setData(sizeof(fArray), fArray);
				IBO.setData(sizeof(iArray), iArray);
				VAO.addAttribute(2, &VBO);
			}
			shader2->bind();
			VAO.draw(&IBO);
		}
		void Test3D(LoopInfo& info) {
			if (!shader3) {
				shader3 = ALLOC_NEW(Shader)(test3dGLSL);
				float fArray[]{
					0,0,-4,
					1,0,-4,
					1,1,-4
				};
				uint32_t iArray[]{
					0,1,2,
				};
				VBO.setData(sizeof(fArray), fArray);
				IBO.setData(sizeof(iArray), iArray);
				VAO.addAttribute(3, &VBO);
				init_test3d = true;
			}
			EnableBlend();
			shader3->bind();
			Renderer* renderer = info.window->getRenderer();
			renderer->updateProjection(shader3);
			VAO.draw(&IBO);
		}
		static const char* particlesGLSL = {
#include "Engone/Tests/testParticle.glsl"
		};
		static ParticleGroup<DefaultParticle> partGroup{};
		static Shader* shaderPart = nullptr;
		void TestParticles(LoopInfo& info) {
			if (!shaderPart) {
				shaderPart = ALLOC_NEW(Shader)(particlesGLSL);
				partGroup.init(info.window, shaderPart);
				//partGroup.resize(10000000);
				//uint32_t count = 18*pow(10,6);
				uint32_t count = 1*pow(10,6);
				//log::out << count << "\n";
				DefaultParticle* parts = partGroup.createParticles(count);
				if (parts) {
					glm::vec3 bounds = { 5,5,5 };
					glm::vec3 center = {4,0,0};
									
					for (int i = 0; i < count; i++) {
						float x = GetRandom();
						float y = GetRandom();
						float z = GetRandom();
						//float x = 0;
						//float y = 0;
						//float z = 0;
						parts[i].pos = { center.x+bounds.x*(x-0.5), center.y + bounds.y * (y-0.5), center.z + bounds.z * (z-0.5)};
						parts[i].vel = { 0, 0, 1 };
					}
				}
			}
			if (shaderPart) {
				shaderPart->bind();
				shaderPart->setVec3("focusPoint", { 0,0,0 });
				shaderPart->setFloat("delta", 1.f/60.f);
				//Timer rend("part");
				partGroup.render(info);
				//rend.stop();
			}
		}
	}
}