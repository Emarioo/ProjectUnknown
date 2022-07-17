#include "Engone/Rendering/Buffer.h"
#include "Engone/AssetModule.h"
#include "Engone/Engone.h"

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
		void Test2D(RenderInfo& info) {
			if (!shader2) {
				shader2 = new Shader(test2dGLSL);
				float fArray[]{
					0,0,
					1,0,
					1,1
				};
				uint32_t iArray[]{
					0,1,2,
				};
				VBO.setData(sizeof(fArray) / sizeof(float), fArray);
				IBO.setData(sizeof(iArray) / sizeof(uint32_t), iArray);
				VAO.addAttribute(2, &VBO);
			}
			shader2->bind();
			VAO.draw(&IBO);
		}
		void Test3D(RenderInfo& info) {
			if (!shader3) {
				shader3 = new Shader(test3dGLSL);
				float fArray[]{
					0,0,-4,
					1,0,-4,
					1,1,-4
				};
				uint32_t iArray[]{
					0,1,2,
				};
				VBO.setData(sizeof(fArray) / sizeof(float), fArray);
				IBO.setData(sizeof(iArray) / sizeof(uint32_t), iArray);
				VAO.addAttribute(3, &VBO);
				init_test3d = true;
			}
			EnableBlend();
			shader3->bind();
			Renderer* renderer = info.window->getRenderer();
			renderer->updateViewMatrix(0);
			renderer->updateProjection(shader3);
			VAO.draw(&IBO);
		}
	}
}