#include "gonpch.h"

#include "UIPipeline.h"

#include "../Rendering/Buffer.h"

#include "../Handlers/AssetHandler.h"

#include "../EventHandler.h"

#include "../Window.h"

static const char* uiPipelineGLSL = {
#include "../Shaders/uiPipeline.glsl"
};

namespace engone {

	static const int MAX_RECT_BATCH = 100;
	static const int VERTEX_SIZE = 6;
	static VertexArray vertexArray;
	static VertexBuffer rectBuffer;
	static IndexBuffer indexBuffer;
	static float floatArray[4 * VERTEX_SIZE *MAX_RECT_BATCH];

	static Pipeline uiPipeline;

	// global ui pipeline
	namespace ui {
		
		// There is a limit to how many rects you can have thanks to float[], having a std::vector and batching rendering would be better
		void Rect(Box box) {
			uiPipeline.Rect(box);
		}
		void String(Text text) {
			uiPipeline.Text(text);
		}

		static bool inside(Box box, float x, float y) {
			return box.x<x&& box.x + box.w>x && box.y<y && box.y + box.h>y;
		}
		bool Clicked(Box box) {
			if (IsKeyPressed(GLFW_MOUSE_BUTTON_1)) {
				float x = GetMouseX();
				float y = GetMouseY();
				if (inside(box,x,y)) {
					return true;
				}
			}
			return false;
		}
		bool Hover(Box box) {
			if (IsKeyDown(GLFW_MOUSE_BUTTON_1)) {
				float x = GetMouseX();
				float y = GetMouseY();
				if (inside(box, x, y)) {
					return true;
				}
			}
			return false;
		}
	}
	void InitUIPipeline() {

		AddAsset<Shader>("uiPipeline", new Shader(uiPipelineGLSL));

		rectBuffer.setData(4 * VERTEX_SIZE * MAX_RECT_BATCH, nullptr);

		uint32_t intArray[6 * MAX_RECT_BATCH];
		for (int i = 0; i < MAX_RECT_BATCH; i++) {
			intArray[i * 6] = i*4+0;
			intArray[i * 6 + 1] = i*4+1;
			intArray[i * 6 + 2] = i*4+2;
			intArray[i * 6 + 3] = i*4+2;
			intArray[i * 6 + 4] = i*4+3;
			intArray[i * 6 + 5] = i*4+0;
		}
		indexBuffer.setData(6 * MAX_RECT_BATCH, intArray);

		//vertexArray.addAttribute(2, &rectBuffer);
		vertexArray.addAttribute(2); // VERTEX_SIZE doesn't really work here
		vertexArray.addAttribute(4, &rectBuffer);
	}
	static struct VERTEX {
		float data[VERTEX_SIZE];
	};
	void RenderUIPipeline() {
		// setup
		Shader* shad = GetAsset<Shader>("uiPipeline");
		if (!shad) return;

		EnableBlend();
		shad->bind();
		shad->setVec2("uWindow", { GetWidth(), GetHeight() });

		/*log::out << "RectCount " << rectCount << "\n";

		for (int i = 0; i < MAX_RECT_BATCH;i++) {
			for (int j = 0; j < 4; j++)
				log::out << floatArray[i*4*2+j*2] <<" "<< floatArray[i * 4 * 2 + j * 2 + 1];
			log::out << "\n";
		}*/
		// render buffers
		Pipeline& pipe = uiPipeline;
		if (pipe.boxes.size() > 0) {
			int totalDrawn = 0;
			while (totalDrawn< pipe.boxes.size()) {
				int nowDrawn = 0;
				while(nowDrawn < MAX_RECT_BATCH && totalDrawn< pipe.boxes.size()) {
					Box& box = pipe.boxes[totalDrawn];
					//log::out << box.x <<" [] "<<totalDrawn << "\n";
					((VERTEX*)&floatArray)[nowDrawn * 4 + 0] = { box.x, box.y, box.r, box.g, box.b, box.a };
					((VERTEX*)&floatArray)[nowDrawn * 4 + 1] = { box.x, box.y + box.h, box.r, box.g, box.b, box.a };
					((VERTEX*)&floatArray)[nowDrawn * 4 + 2] = { box.x + box.w, box.y + box.h, box.r, box.g, box.b, box.a };
					((VERTEX*)&floatArray)[nowDrawn * 4 + 3] = { box.x + box.w, box.y, box.r, box.g, box.b, box.a };
					nowDrawn++;
					totalDrawn++;
				}

				/*for (int i = 0; i < 5; i++) {
					for (int j = 0; j < 4; j++)
						log::out << floatArray[i * 4 * VERTEX_SIZE + j * VERTEX_SIZE] << " " << floatArray[i * 4 * VERTEX_SIZE + j * VERTEX_SIZE + 1]<<" ";
					log::out << "\n";
				}*/

				//log::out << "Rect drawn: " << nowDrawn << "\n";

				if(nowDrawn!=MAX_RECT_BATCH)
					ZeroMemory(floatArray + nowDrawn * 4 * VERTEX_SIZE, (MAX_RECT_BATCH - nowDrawn) * 4 * VERTEX_SIZE * sizeof(float));

				rectBuffer.setData(MAX_RECT_BATCH * 4 * VERTEX_SIZE, floatArray);
				vertexArray.draw(&indexBuffer);
			}

			// cleanup
			pipe.boxes.clear();
		}
	}
}