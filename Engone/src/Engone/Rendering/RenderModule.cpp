
#include "Engone/RenderModule.h"

#include "Engone/Window.h"
#include "Engone/EventModule.h"

#include "Engone/Engone.h"
#ifdef gone
static const char* uiPipelineGLSL = {
#include "Engone/Shaders/uiPipeline.glsl"
};
static const char* rendererGLSL = {
#include "Engone/Shaders/renderer.glsl"
};
static const char* linesGLSL = {
#include "Engone/Shaders/lines3d.glsl"
};
static const char* guiShaderSource = {
#include "Engone/Shaders/gui.glsl"
};
static const char* objectSource = {
#include "Engone/Shaders/object.glsl"
};
static const char* armatureSource = {
#include "Engone/Shaders/armature.glsl"
};
static const char* particleGLSL= {
#include "Engone/Shaders/particle.glsl"
};

namespace engone {

#ifndef PIPE3_LINE_BATCH_LIMIT
#define PIPE3_LINE_BATCH_LIMIT 500
#endif
#ifndef PIPE3_LINE_RESERVE
#define PIPE3_LINE_RESERVE PIPE3_LINE_BATCH_LIMIT
#endif

	static Renderer* g_activeRenderer = nullptr;
	void Renderer::init() {
		if (m_initialized) return;
		m_initialized = true;
		m_parent = GetActiveWindow();

		//m_parent = GetActiveWindow();
		uint32_t indes[TEXT_BATCH * 6];
		for (int i = 0; i < TEXT_BATCH; i++) {
			indes[0 + 6 * i] = 0 + 4 * i;
			indes[1 + 6 * i] = 1 + 4 * i;
			indes[2 + 6 * i] = 2 + 4 * i;
			indes[3 + 6 * i] = 2 + 4 * i;
			indes[4 + 6 * i] = 3 + 4 * i;
			indes[5 + 6 * i] = 0 + 4 * i;
		}
		textIBO.setData(6 * TEXT_BATCH*sizeof(float), indes);
		textVBO.setData(16 * TEXT_BATCH * sizeof(float), nullptr);
		textVAO.addAttribute(4, &textVBO);

		//Assets* assets = m_parent->getAssets();
		AssetStorage* assets = m_parent->getStorage();

		// pipeline

		assets->set<ShaderAsset>("uiPipeline", ALLOC_NEW(ShaderAsset)(uiPipelineGLSL));

		boxVBO.setData(4 * VERTEX_SIZE * MAX_BOX_BATCH * sizeof(float), nullptr);

		uint32_t intArray[6 * MAX_BOX_BATCH];
		for (int i = 0; i < MAX_BOX_BATCH; i++) {
			intArray[i * 6] = i * 4 + 0;
			intArray[i * 6 + 1] = i * 4 + 1;
			intArray[i * 6 + 2] = i * 4 + 2;
			intArray[i * 6 + 3] = i * 4 + 2;
			intArray[i * 6 + 4] = i * 4 + 3;
			intArray[i * 6 + 5] = i * 4 + 0;
		}
		boxIBO.setData(6 * MAX_BOX_BATCH * sizeof(float), intArray);

		boxVAO.addAttribute(2); // pos
		boxVAO.addAttribute(2); // uv
		boxVAO.addAttribute(4); // color
		boxVAO.addAttribute(1, &boxVBO); // texture

		float near_plane = 1.f, far_plane = 20.5f;
		lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);

		setProjection();

		// ISSUE: how to deal with tracking of assets, when window is destroyed, free assets.
		//	What if you have added an asset twice with different names
		//  Freeing assets not allocate with tracker would give negative memoray allocations.
		//  Either the user needs to define some flag?
		//  Maybe track with addMemory event if assets are added twice it would be fine?
		//  maybe use tracker inside constructors, You can't have inside constructors because a stack allocated class would be tracked.

		// Line pipeline
		assets->set<ShaderAsset>("lines3d", ALLOC_NEW(ShaderAsset)(linesGLSL));
		pipe3lines.reserve(PIPE3_LINE_RESERVE*12);
		pipe3lineVB.setData(PIPE3_LINE_BATCH_LIMIT*12 * sizeof(float), nullptr);
		uint32_t indLine[PIPE3_LINE_BATCH_LIMIT*2];
		for (int i = 0; i < PIPE3_LINE_BATCH_LIMIT * 2; i++) {
			indLine[i] = i;
		}
		pipe3lineIB.setData(PIPE3_LINE_BATCH_LIMIT*2 * sizeof(float), indLine);
		pipe3lineVA.addAttribute(3);
		pipe3lineVA.addAttribute(3, &pipe3lineVB);

		assets->set<ShaderAsset>("renderer", ALLOC_NEW(ShaderAsset)(rendererGLSL));
		float cubeVertices[48]{ // this is correct, the shader must be wrong. The latest index's normal is used.
			0,0,0, 0,0,0,
			1,0,0, 0,0,-1,
			0,1,0, -1,0,0,
			1,1,0, 0,1,0,

			0,0,1, 0,-1,0,
			1,0,1, 1,0,0,
			0,1,1, 0,0,1,
			1,1,1, 0,0,0,
		};
		for (int i = 0; i < 24; i++)
			cubeVertices[i % 3 + (i / 3) * 6] -= 0.5;
		cubeVBO.setData(48*sizeof(float), cubeVertices);
		uint32_t cubeIndices[36]{
			0, 2, 1, 2, 3, 1,
			0, 1, 4, 1, 5, 4,
			0, 4, 2, 4, 6, 2,
			2, 6, 3, 6, 7, 3,
			1, 3, 5, 3, 7, 5,
			4, 5, 6, 5, 7, 6,
		};
		cubeIBO.setData(36 * sizeof(uint32_t), cubeIndices);

		cubeInstanceVBO.setData(MAX_BOX_BATCH * sizeof(Cube), nullptr);
		cubeVAO.addAttribute(3);
		cubeVAO.addAttribute(3, &cubeVBO);
		cubeVAO.addAttribute(4, 1);
		cubeVAO.addAttribute(4, 1);
		cubeVAO.addAttribute(4, 1);
		cubeVAO.addAttribute(4, 1);
		cubeVAO.addAttribute(3, 1, &cubeInstanceVBO);

		float cube2Vertices[24]{ // this is correct, the shader must be wrong. The latest index's normal is used.
			0,0,0,
			1,0,0,
			0,1,0,
			1,1,0,

			0,0,1,
			1,0,1,
			0,1,1,
			1,1,1,
		};
		for (int i = 0; i < 24; i++)
			cube2Vertices[i] -= 0.5;
		cube2VBO.setData(sizeof(cube2Vertices), cube2Vertices);
		uint32_t cube2Indices[36]{
			0, 2, 1, 2, 3, 1,
			0, 1, 4, 1, 5, 4,
			0, 4, 2, 4, 6, 2,
			2, 6, 3, 6, 7, 3,
			1, 3, 5, 3, 7, 5,
			4, 5, 6, 5, 7, 6,
		};
		cube2IBO.setData(36 * sizeof(uint32_t), cube2Indices);
		cube2VAO.addAttribute(3, &cube2VBO);

		float simpleQuad[]{
			0,1,0,1,
			0,0,0,0,
			1,1,1,1,
			1,0,1,0,
			1,1,1,1,
			0,0,0,0,

			//0,0,0,1,
			//0,1,0,0,
			//1,1,1,0,
			//1,0,1,1,
		};
		//uint32_t simpleInd[]{
		//	0, 1, 2, 2, 3, 0,
		//};
		quadVB.setData(sizeof(simpleQuad), simpleQuad);
		//quadIB.setData(sizeof(uint32_t)*6, simpleInd);
		quadVA.addAttribute(4,&quadVB);

		instanceBuffer.setData(INSTANCE_BATCH * sizeof(glm::mat4), nullptr);

		assets->set<ShaderAsset>("gui", ALLOC_NEW(ShaderAsset)(guiShaderSource));
		
		assets->set<ShaderAsset>("object", ALLOC_NEW(ShaderAsset)(objectSource));
		assets->set<ShaderAsset>("armature", ALLOC_NEW(ShaderAsset)(armatureSource));
		MaterialAsset* mat = ALLOC_NEW(MaterialAsset)();

		//mat->setBaseName("defaultMaterial");
		assets->set<MaterialAsset>("defaultMaterial", mat);

		//instanceBuffer.setData(INSTANCE_LIMIT * 16, nullptr);
		//depthBuffer.init();

		//AddListener(new Listener(EventType::Resize, 9999, DrawOnResize));
		setActiveRenderer();
	}
	void Renderer::DrawQuad(LoopInfo& info) {
		//Assets* assets = info.window->getAssets();
		//Shader* guiShader = assets->get<Shader>("gui");

		//guiShader->bind();
		//guiShader->setVec2("uWindow", { GetWidth(), GetHeight() });
		//guiShader->setVec2("uPos", { x,y });
		//guiShader->setVec2("uSize", {w,h});
		//guiShader->setVec4("uColor", 1,1,1,1);
		//guiShader->setInt("uTextures",0);
		//guiShader->setInt("uColorMode",texture?1:0);

		//quadVA.draw(&quadIB);
		quadVA.drawTriangleArray(6);
	}
	void TerminateRenderer() {
		// destroy buffers
		//colliderBuffer.Uninit();
		//lineBuffer.Uninit();
	}
	void EnableBlend() {
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}
	void EnableDepth() {
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
	Camera* Renderer::getCamera() {
		return &camera;
	}
	glm::mat4& Renderer::getLightProj() {
		return lightProjection;
	}
	void Renderer::setProjection() {
		//if (ratio == -NAN) log::out << "Renderer::SetProjection ratio was bad\n";
		float ratio = GetWidth() / GetHeight();
		if (isfinite(ratio))
			projMatrix = glm::perspective(fov, ratio, zNear, zFar);
	}
	glm::mat4& Renderer::getProjection() {
		return projMatrix;
	}
	void Renderer::setOrthogonal() {
		//float ratio = GetWidth() / GetHeight();
		//projMatrix = glm::ortho(-ratio, ratio, -1.f, 1.f, zNear, zFar);
		// you need to apply the ratio to the models yourself
		projMatrix = glm::ortho(-1.f, 1.f, -1.f, 1.f, zNear, zFar);
	}
	// Should be done once in the render loop, currently done in Engone::render
	//void Renderer::updateViewMatrix(double lag) {
	//	viewMatrix = glm::inverse(
	//		glm::translate(glm::mat4(1.0f), getCamera()->position) *
	//		glm::rotate(getCamera()->rotation.y, glm::vec3(0, 1, 0)) *
	//		glm::rotate(getCamera()->rotation.x, glm::vec3(1, 0, 0))
	//	);
	//}
	void Renderer::updateProjection(Shader* shader) {
		if (shader != nullptr)
			shader->setMat4("uProj", projMatrix * camera.getViewMatrix());
	}
	void Renderer::updateProjection(Shader* shader, glm::mat4 viewMatrix) {
		if (shader != nullptr)
			shader->setMat4("uProj", projMatrix * viewMatrix);
	}
	static void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3) {
		ar[ind] = f0;
		ar[ind + 1] = f1;
		ar[ind + 2] = f2;
		ar[ind + 3] = f3;
	}
	void Renderer::DrawString(FontAsset* font, const std::string& text, bool center, float wantedHeight, float maxWidth, float maxHeight, int atChar) {
		if (text.length() == 0 && atChar == -1)
			return;
		if (font == nullptr) {
			std::cout << "DrawString: Font is null\n";
			return;
		} else if (font->getError()) {
			return;
		} else {
			//Shader* guiShader = m_parent->getAssets()->get<Shader>("gui");
			ShaderAsset* guiShader = m_parent->getStorage()->get<ShaderAsset>("gui");
			if (guiShader != nullptr)
				guiShader->setInt("uColorMode", 1);
			//guiShader->setInt("uTextures", 0);
			font->texture.bind();
		}

		std::vector<std::string> lines;
		std::vector<float> lineWidths;

		float reachedWidth = 0;
		float spacing = 0;
		int lineIndex = 0;
		std::string word;
		float wordWidth = 0;
		lines.push_back("");
		lineWidths.push_back(0);
		for (uint32_t i = 0; i < text.length(); ++i) {
			unsigned char chr = text[i];
			//std::cout << (int)chr << "\n";

			float added = wantedHeight * (font->charWid[chr] / (float)font->charSize);
			//if (wordWidth != 0 && lineWidths[lineIndex] != 0)
			added += spacing;

			/*
			if (wordWidth==0&&lineWidths[lineIndex] == 0 && chr == ' ') {// skip space in the beginning of line
				continue;
			}
			*/

			if (chr == '\n') {// || chr == ' ' && lineWidths[lineIndex] + wordWidth + added > reachedWidth) { // new line
				//if (lineWidths[lineIndex] + wordWidth < maxWidth) {
				lines[lineIndex] += word;
				lineWidths[lineIndex] += wordWidth;
				if (chr == ' ') {
					lines[lineIndex] += ' ';
					lineWidths[lineIndex] += added;
				}
				//if (chr == '\n')
				//	lines[lineIndex] += '\n';
				//if (i!=text.length()-1) {
				lineWidths.push_back(0);
				//	if(chr=='\n')
				lines.push_back("\n");
				//	else
				//		lines.push_back("");
				//}
				//}
				//else {
				//	lineWidths.push_back(wordWidth);
				//	lines.push_back(word);
				//}
				word = "";
				wordWidth = 0;
				lineIndex++;
				continue;
			}
			/*
			if (lineWidths[lineIndex]+ wordWidth + added >maxWidth) {
				i--;

				if (word[0] == ' ') {
					word = word.substr(1);
					wordWidth -= spacing+wantedHeight * (font->charWid[chr] / (float)font->charSize);
				}

				lineWidths.push_back(wordWidth);
				lines.push_back(word);
				word = "";
				wordWidth = 0;
				lineIndex++;
				continue;
			}
			*/
			/*
			if (chr == ' ') {
				lines[lineIndex] += word;
				lineWidths[lineIndex] += wordWidth;
				word = "";
				wordWidth = 0;
			}
			*/
			word += chr;
			wordWidth += added;

			// add the last stuff if the last character is reached
			if (i == text.size() - 1) {
				lines[lineIndex] += word;
				lineWidths[lineIndex] += wordWidth;
			}
			// get the maximum width
			if (wordWidth > reachedWidth) {
				reachedWidth = wordWidth;
			}
		}
		//std::cout << "Lines " << lineWidths.size() << " " << lines.size() << "\n";

		if (reachedWidth > maxWidth) {
			wantedHeight *= maxWidth / reachedWidth;
			for (uint32_t i = 0; i < lineWidths.size(); ++i) {
				//lineWidths[i] -= spacing * (lines[i].length() - 1);
				lineWidths[i] *= maxWidth / reachedWidth;
				//lineWidths[i] += spacing*(lines[i].length()-1);
				//std::cout << rowWidths[i] << "\n";
			}
		}
		if (lineWidths.size() * wantedHeight > maxHeight) {
			for (uint32_t i = 0; i < lineWidths.size(); ++i) {
				//lineWidths[i] -= spacing * (lines[i].length() - 1);
				lineWidths[i] *= maxHeight / (lineWidths.size() * wantedHeight);
				//lineWidths[i] += spacing*(lines[i].length()-1);
				//std::cout << rowWidths[i] << "\n";
			}
			wantedHeight = maxHeight / lineWidths.size();
		}
		//std::cout << wantedHeight << "\n";
		float x = 0;
		float y = 0;
		if (center) {
			y = (maxHeight - wantedHeight * lines.size()) / 2;
		}
		uint32_t dataIndex = 0;
		//std::cout << lines[0] << "\n";
		if (atChar != -1) { // do marker
			//std::cout << lines.size()<<"\n";
			for (uint32_t i = 0; i < lines.size(); ++i) {
				if (center)
					x = (maxWidth - lineWidths[i]) / 2;
				else
					x = 0;
				for (uint32_t j = 0; j < lines[i].length(); ++j) {
					char chr = lines[i][j];

					float wStride = wantedHeight * (font->charWid[chr] / (float)font->charSize);

					if (atChar == dataIndex) {
						break;
					}
					x += wStride + spacing;

					dataIndex++;
				}
				if (atChar == dataIndex)
					break;
				y += wantedHeight;
			}
			if (atChar != dataIndex) {
				if (center)
					x = maxWidth / 2;
				else
					x = 0;
			}
			dataIndex = 0;
			//std::cout << x << "\n";
			float wuv = font->charWid[0] / (float)font->imgSize;
			float u = (0 % 16);
			float v = 15 - (0 / 16);
			float markerW = wantedHeight * (font->charWid[0] / (float)font->charSize);
			Insert4(verts, 16 * dataIndex, x, y, (u) / 16, (v + 1) / 16);
			Insert4(verts, 4 + 16 * dataIndex, x, y + wantedHeight, (u) / 16, (v) / 16);
			Insert4(verts, 8 + 16 * dataIndex, x + markerW, y + wantedHeight, (u) / 16 + wuv, (v) / 16);
			Insert4(verts, 12 + 16 * dataIndex, x + markerW, y, (u) / 16 + wuv, (v + 1) / 16);
			dataIndex = 1;
			if (center) {
				y = (maxHeight - wantedHeight * lines.size()) / 2;
			}
		}

		for (uint32_t i = 0; i < lines.size(); ++i) {
			//std::cout << "[" << lines[i] << "] " << "\n";
			float x = 0;
			if (center)
				x = (maxWidth - lineWidths[i]) / 2;
			//std::cout << lines[i].length()<<"\n";
			for (uint32_t j = 0; j < lines[i].length(); ++j) {
				uint8_t chr = lines[i][j];
				//if (chr == '\n')
				//	continue;
				float wStride = wantedHeight * (font->charWid[chr] / (float)font->charSize);

				float wuv = font->charWid[chr] / (float)font->imgSize;
				float u = (float)(chr % 16);
				float v = (float)(15 - (chr / 16));

				Insert4(verts, 16 * dataIndex, x, y, (u) / 16, (v + 1) / 16);
				Insert4(verts, 4 + 16 * dataIndex, x, y + wantedHeight, (u) / 16, (v) / 16);
				Insert4(verts, 8 + 16 * dataIndex, x + wStride, y + wantedHeight, (u) / 16 + wuv, (v) / 16);
				Insert4(verts, 12 + 16 * dataIndex, x + wStride, y, (u) / 16 + wuv, (v + 1) / 16);
				x += wStride + spacing;

				if (dataIndex + 1 == TEXT_BATCH) {
					textVBO.setData(16 * TEXT_BATCH*sizeof(float), verts);
					textVAO.draw(&textIBO);
					//textBuffer.ModifyVertices(0, 4 * 4 * TEXT_BATCH, verts);
					//textBuffer.Draw();
					dataIndex = 0;
				} else {
					dataIndex++;
				}
			}
			y += wantedHeight;
		}

		uint32_t charIndex = dataIndex % TEXT_BATCH;
		memset(verts+16 * charIndex, 0, 16 * (TEXT_BATCH - charIndex) * sizeof(float));

		//memset(&verts[0], 0, 16 * (TEXT_BATCH) * sizeof(float));
		/*
		Insert4(verts,0, 0, 0, 0, 0);
		Insert4(verts,4, 1, 0, 0, 1);
		Insert4(verts,8, 1, 1, 1, 1);
		Insert4(verts,12, 0, 1, 1, 0);

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				std::cout << verts[i * 4 + j]<<" ";
			}
			std::cout << "\n";
		}*/

		textVBO.setData(16 * TEXT_BATCH * sizeof(float), verts);
		textVAO.draw(&textIBO);

		//textBuffer.ModifyVertices(0, 16 * TEXT_BATCH, verts);
		//textBuffer.Draw();
		//rectBuffer.Draw();
	}
	void Renderer::DrawString(FontAsset* font, const std::string& text, float height, int cursorPosition) {
		//log::out << text.length() << " " << cursorPosition << "\n";

		if (text.length() == 0 && cursorPosition == -1)
			return;
		if (font == nullptr) {
			std::cout << "DrawString: Font is null\n";
			return;
		} else if (font->getError()) {
			return;
		} else {
			//Shader* guiShader = m_parent->getAssets()->get<Shader>("gui");
			ShaderAsset* guiShader = m_parent->getStorage()->get<ShaderAsset>("gui");
			if (guiShader != nullptr)
				guiShader->setInt("uColorMode", 1);
			//guiShader->setInt("uTextures", 0);
			font->texture.bind();
		}


		float spacing = 0;

		float x = 0;
		float y = 0;
		uint32_t dataIndex = 0;
		//log::out << "pos " << cursorPosition << "\n";
		if (cursorPosition != -1) { // do marker
			//log::out << "pos "<<cursorPosition << "\n";
			for (uint32_t j = 0; j < text.size(); ++j) {
				uint8_t chr = text[j];
				if (chr == '\n') {
					x = 0;
					y += height;
					continue;
				}

				float wStride = height * (font->charWid[chr] / (float)font->charSize);

				if (cursorPosition == dataIndex) {
					break;
				}
				x += wStride + spacing;

				dataIndex++;
			}
			if (cursorPosition != dataIndex) {
				log::out << log::YELLOW<<"Renderer::DrawString - Bad code when drawing text\n";
				// you may have
				//log::out << cursorPosition << " " << text.size() << "\n";
				x = 0;
			}
			dataIndex = 0;
			//std::cout << x << "\n";
			float wuv = font->charWid[0] / (float)font->imgSize;
			float u = (0 % 16);
			float v = 15 - (0 / 16);
			float markerW = height * (font->charWid[0] / (float)font->charSize);
			Insert4(verts, 16 * dataIndex, x, y, (u) / 16, (v + 1) / 16);
			Insert4(verts, 4 + 16 * dataIndex, x, y + height, (u) / 16, (v) / 16);
			Insert4(verts, 8 + 16 * dataIndex, x + markerW, y + height, (u) / 16 + wuv, (v) / 16);
			Insert4(verts, 12 + 16 * dataIndex, x + markerW, y, (u) / 16 + wuv, (v + 1) / 16);
			dataIndex = 1;
		}
		x = 0;
		//dataIndex = 0;
		//std::cout << lines[i].length()<<"\n";
		for (uint32_t j = 0; j< text.size(); ++j) {
			uint8_t chr = text[j];
			if (chr == '\n') {
				x = 0;
				y += height;
				continue;
			}
			float wStride = height * (font->charWid[chr] / (float)font->charSize);

			float wuv = font->charWid[chr] / (float)font->imgSize;
			float u = (float)(chr % 16);
			float v = (float)(15 - (chr / 16));

			Insert4(verts, 16 * dataIndex, x, y, (u) / 16, (v + 1) / 16);
			Insert4(verts, 4 + 16 * dataIndex, x, y + height, (u) / 16, (v) / 16);
			Insert4(verts, 8 + 16 * dataIndex, x + wStride, y + height, (u) / 16 + wuv, (v) / 16);
			Insert4(verts, 12 + 16 * dataIndex, x + wStride, y, (u) / 16 + wuv, (v + 1) / 16);
			x += wStride + spacing;

			if (dataIndex + 1 == TEXT_BATCH) {
				textVBO.setData(16 * TEXT_BATCH * sizeof(float), verts);
				textVAO.draw(&textIBO);
				dataIndex = 0;
			} else {
				dataIndex++;
			}
		}

		uint32_t charIndex = dataIndex % TEXT_BATCH;
		memset(verts + 16 * charIndex, 0, 16 * (TEXT_BATCH - charIndex) * sizeof(float));

		textVBO.setData(16 * TEXT_BATCH * sizeof(float), verts);
		textVAO.draw(&textIBO);
	}
	void Renderer::DrawCube(glm::mat4 matrix, glm::vec3 scale, glm::vec3 color) {
		cubeObjects.push_back({ glm::scale(matrix,scale),color });
	}
	void Renderer::DrawCubeRaw() {
		cube2VAO.draw(&cube2IBO);
	}
	void Renderer::DrawNetCube(glm::mat4 matrix, glm::vec3 scale, glm::vec3 color) {
		auto p = [&](float x, float y, float z) {
			return (matrix * glm::translate(glm::vec3(scale.x * (x - .5), scale.y * (y - .5), scale.z * (z - .5))))[3];
		};
		DrawLine(p(0, 0, 0), p(1, 0, 0),color);
		DrawLine(p(0, 0, 0), p(0, 0, 1),color);
		DrawLine(p(1, 0, 1), p(1, 0, 0),color);
		DrawLine(p(1, 0, 1), p(0, 0, 1),color);

		DrawLine(p(0, 0, 0), p(0, 1, 0),color);
		DrawLine(p(1, 0, 0), p(1, 1, 0),color);
		DrawLine(p(1, 0, 1), p(1, 1, 1),color);
		DrawLine(p(0, 0, 1), p(0, 1, 1),color);

		DrawLine(p(0, 1, 0), p(1, 1, 0),color);
		DrawLine(p(0, 1, 0), p(0, 1, 1),color);
		DrawLine(p(1, 1, 1), p(1, 1, 0),color);
		DrawLine(p(1, 1, 1), p(0, 1, 1),color);
	}
	constexpr float pis() {
		return glm::pi<float>();
	}
	void Renderer::DrawSphere(glm::vec3 position, float radius, glm::vec3 color) {
	/*	std::vector<glm::vec3> points;
		std::vector<tri> tris;
		std::vector<line> lines;*/

		constexpr float pi = pis();
		//DrawBegin();
		//AddVertex(0, 1*radius, 0);
		//AddVertex(0,-1* radius, 0);
		//for (int i = 0; i < 5; i++) {
		//	AddVertex( cosf(2 * pi * (i / 5.f))*radius,.5f* radius,sinf(2 * pi * (i / 5.f)) * radius);
		//	AddVertex( cosf(2 * pi * (1 / 10.f + i / 5.f))* radius,-.5f* radius,sinf(2 * pi * (1 / 10.f + i / 5.f)) * radius);

		//	AddIndex( 2 + i * 2,2 + i * 2 + 1 );
		//	AddIndex( 2 + ((i + 1) % 5) * 2,2 + i * 2 + 1 );
		//	AddIndex( 2 + i * 2, 2 + ((i + 1) % 5) * 2 );
		//	AddIndex( 2 + i * 2 + 1, 2 + ((i + 1) % 5) * 2 + 1 );
		//	
		//	AddIndex( 0,2 + i * 2 );
		//	AddIndex( 1,2 + i * 2 + 1 );
		//}
		//DrawBuffer();

	}
	//struct line {
	//	int a, b;
	//};
	//struct tri {
	//	int a, b, c;
	//};
	//void DrawSphere(float radius) {
	//	std::vector<glm::vec3> points;
	//	std::vector<tri> tris;
	//	std::vector<line> lines;

	//	float pi = glm::pi<float>();
	//	points.push_back({ 0,1,0 });
	//	points.push_back({ 0,-1,0 });
	//	for (int i = 0; i < 5; i++) {
	//		points.push_back({ cosf(2 * pi * (i / 5.f)),.5,sinf(2 * pi * (i / 5.f)) });
	//		points.push_back({ cosf(2 * pi * (1 / 10.f + i / 5.f)),-.5,sinf(2 * pi * (1 / 10.f + i / 5.f)) });

	//		lines.push_back({ 2 + i * 2,2 + i * 2 + 1 });
	//		lines.push_back({ 2 + ((i + 1) % 5) * 2,2 + i * 2 + 1 });
	//		lines.push_back({ 2 + i * 2, 2 + ((i + 1) % 5) * 2 });
	//		lines.push_back({ 2 + i * 2 + 1, 2 + ((i + 1) % 5) * 2 + 1 });

	//		lines.push_back({ 0,2 + i * 2 });
	//		lines.push_back({ 1,2 + i * 2 + 1 });
	//	}

	//	for (glm::vec3& p : points)
	//		p *= radius;

	//	{
	//		std::vector<glm::vec3> oldPoints = points;
	//		std::vector<line> oldLines = lines;
	//		std::vector<tri> oldTris = tris;
	//	}

	//	/*colliderBuffer.ModifyVertices(0, points.size() * 3, points.data());
	//	colliderBuffer.ModifyIndices(0, lines.size() * 2, lines.data());
	//	colliderBuffer.Draw();*/
	//}
	void Renderer::DrawLine(glm::vec3 a, glm::vec3 b, glm::vec3 rgb) {
		pipe3lines.push_back(a.x);
		pipe3lines.push_back(a.y);
		pipe3lines.push_back(a.z);
		pipe3lines.push_back(rgb.x);
		pipe3lines.push_back(rgb.y);
		pipe3lines.push_back(rgb.z);

		pipe3lines.push_back(b.x);
		pipe3lines.push_back(b.y);
		pipe3lines.push_back(b.z);
		pipe3lines.push_back(rgb.x);
		pipe3lines.push_back(rgb.y);
		pipe3lines.push_back(rgb.z);
	}
	void Renderer::DrawTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
		DrawLine(a,b);
		DrawLine(b,c);
		DrawLine(c,a);
	}
	//void Renderer::DrawOrthoModel(ModelAsset* modelAsset, glm::mat4 matrix) {
	//	modelObjects.push_back({ modelAsset, matrix, true });
	//}
	//void Renderer::DrawModel(ModelAsset* modelAsset, glm::mat4 matrix) {
	//	modelObjects.push_back({ modelAsset, matrix, false});
	//}
	// global ui pipeline
	namespace ui {
		float TextBox::getWidth() {
			if (!font) return 0;
			return font->getWidth(text, h);
		}
		float TextBox::getHeight() {
			if (!font) return 0;
			return font->getHeight(text, h);
		}
		struct PipeTextBox {
			//TextBox(const std::string& text, float x, float y, float h) : text() {}
			uint32_t text_index=0;
			float x = 0, y = 0, h = 20;
			FontAsset* font = nullptr;
			Color rgba;
			uint32_t at = -1;
		};
		struct ModelBox {
			uint32_t model_text_index = 0;
			glm::mat4 matrix;
		};
		void Draw(Box box) {
			if (!GetActiveRenderer()) return;
			if (box.x + box.w < 0 || box.x>GetWidth()||box.y+box.h<0||box.y>GetHeight())
				return; // out of bounds
			GetActiveRenderer()->uiObjects.writeMemory<Box>('B', &box);
		}
		void Draw(TexturedBox box) {
			if (box.texture == nullptr) {
				return;
			}
			if (!GetActiveRenderer()) return;
			if (box.x + box.w < 0 || box.x>GetWidth() || box.y + box.h<0 || box.y>GetHeight())
				return; // out of bounds
			GetActiveRenderer()->uiObjects.writeMemory<TexturedBox>('T', &box);
		}
		void Draw(TextBox& box) {
			if (!box.editing) {
				box.at = -1;
			}
			if (!GetActiveRenderer()) return;
			// ISSUE: width of str is used.  ( box.x+box.w<0 )
			float h = box.getHeight();
			if (box.x>GetWidth() || box.y + h<0 || box.y>GetHeight())
				return; // out of bounds
			PipeTextBox a;
			GetActiveRenderer()->uiStrings.push_back(box.text);
			a.text_index = GetActiveRenderer()->uiStrings.size()-1;
			a.x = box.x;
			a.y = box.y;
			a.h = box.h;
			a.font = box.font;
			a.rgba = box.rgba;
			a.at = box.at;

			GetActiveRenderer()->uiObjects.writeMemory<PipeTextBox>('S', &a);
		}
		void Draw(ModelAsset* asset, glm::mat4 matrix) {
			if (!GetActiveRenderer()) return;
			ModelBox box = { 0,matrix };
			GetActiveRenderer()->uiStrings.push_back(asset->getLoadName());
			box.model_text_index = GetActiveRenderer()->uiStrings.size() - 1;
			GetActiveRenderer()->uiObjects.writeMemory<ModelBox>('M', &box);
		}

		void Edit(std::string& str, int& at,bool& editing, bool stopEditWithEnter) {
			if (at < 0) at = str.length();
			if (at > str.length()) at = str.length();
			uint32_t chr=0;
			if (IsKeyDown(GLFW_KEY_LEFT_CONTROL) && IsKeyPressed(GLFW_KEY_V)) {
				std::string tmp = PollClipboard();
				//str.insert(str.begin() + at, tmp);
				str.insert(at, tmp);
				at += (int)tmp.length();
				return;
			}
			while (chr = PollChar()) {
				if (chr == GLFW_KEY_BACKSPACE) {
					if (str.length() > 0 && at > 0) {
						str = str.substr(0, at - 1) + str.substr(at);
						at--;
					}
				} else if (chr == GLFW_KEY_ENTER) {
					if (stopEditWithEnter) {
						editing = false;
					} else {
						str.insert(str.begin() + at, '\n');
						at++;
					}
				} else if (chr == GLFW_KEY_DELETE) {
					if ((int)str.length() > at) {
						str = str.substr(0, at) + str.substr(at + 1);
					}
				} else if (chr == GLFW_KEY_LEFT) {
					if (at > 0)
						at--;
				} else if (chr == GLFW_KEY_RIGHT) {
					if (at < (int)str.length())
						at++;
				} else {
					str.insert(str.begin() + at, chr);
					at++;
				}
			}
		}
		void Edit(std::string& str) {
			int at = (int)str.length();
			bool editing=false;
			Edit(str, at,editing,true);
		}
		// will only edit if box->editing is true.
		// the TextBox need to store it's editing value for next frame/update.
		// either make your own isEditing variable and then box->editing=isEditing.
		// or have TextBox in a class or as a static global variable. It's up to you.
		void Edit(TextBox* box,bool stopEditWithEnter) {
			if (box->editing) {
				Edit(box->text, box->at,box->editing, stopEditWithEnter);
			}
		}
		static bool inside(Box box, float x, float y) {
			return box.x<x&& box.x + box.w>x && box.y<y&& box.y + box.h>y;
		}
		static bool inside(TextBox box, float x, float y) {
			float w = box.h * box.text.length();
			if (box.font != nullptr) {
				w = box.font->getWidth(box.text, box.h);
			}
			return box.x<x&& box.x + w>x && box.y<y&& box.y + box.h>y;
		}
		int Clicked(Box& box, int mouseKey) {
			if (IsKeyPressed(mouseKey)) {
				float x = GetMouseX();
				float y = GetMouseY();
				if (inside(box, x, y)) {
					return 1;
				}
				return -1;
			}
			return 0;
		}
		int Clicked(TextBox& box, int mouseKey) {
			if (IsKeyPressed(mouseKey)) {
				float x = GetMouseX();
				float y = GetMouseY();
				if (inside(box, x, y)) {
					// set cursor
					float wid = box.font->getWidth(" ", box.h);
					box.at = ((x - box.x) / wid+0.5f);
					return 1;
				}
				return -1;
			}
			return 0;
		}
		bool Hover(Box& box) {
			float x = GetMouseX();
			float y = GetMouseY();
			if (inside(box, x, y)) {
				return true;
			}
			return false;
		}
		bool Hover(TextBox& box) {
			float x = GetMouseX();
			float y = GetMouseY();
			if (inside(box, x, y)) {
				return true;
			}
			return false;
		}
	}
	struct PIPE_VERTEX {
		float data[Renderer::VERTEX_SIZE];
	};
	static int min(int a, int b) {
		return a < b ? a : b;
	}
	void Renderer::render(LoopInfo& info) {
		GLFWwindow* match = glfwGetCurrentContext();
		//log::out << "Win:"<<info.window << "\n";
		if (!m_parent) {
			log::out << log::RED << "Renderer window was nullptr\n";
			return;
		}
		if (match != m_parent->glfw()) {
			log::out << "Renderer::render - glfw context differs!\n";
		}
		EnableDepth();

		ShaderAsset* shader = m_parent->getStorage()->get<ShaderAsset>("renderer");
		if (shader) {
			shader->bind();
			updateProjection(shader);
			shader->setVec3("uCamera", camera.getPosition());
			uint32_t drawnCubes = 0;
			uint32_t cubeCount = 0;

			getParent()->getParent()->getEngine()->bindLights(shader, { 0,0,0 });

			while (drawnCubes < cubeObjects.size()) {
				cubeCount = min(MAX_BOX_BATCH, cubeObjects.size() - drawnCubes);

				// Cube only consists of floats which is important. may want to zero memory the last bit of the instance buffer.
				cubeInstanceVBO.setData(cubeCount * sizeof(Cube), (cubeObjects.data() + drawnCubes));
				cubeVAO.selectBuffer(2, &cubeInstanceVBO);
				cubeVAO.draw(&cubeIBO, cubeCount);
				drawnCubes += cubeCount;
			}
			cubeObjects.clear();
		}
		shader = m_parent->getStorage()->get<ShaderAsset>("lines3d");
		if (shader) {
			shader->bind();
			updateProjection(shader);
			glLineWidth(2.f);
			//shad->setVec3("uColor", { 0.3,0.8,0.3 });
			shader->setVec3("uColor", { 0.9,0.2,0.2 });

			uint32_t drawnLines = 0;

			while (drawnLines * 12 < pipe3lines.size()) {
				// lines to draw
				uint32_t lineCount = min(PIPE3_LINE_BATCH_LIMIT, pipe3lines.size() / 12 - drawnLines);
				
				if (lineCount < PIPE3_LINE_BATCH_LIMIT) {
					int diff = pipe3lines.size() % (PIPE3_LINE_BATCH_LIMIT * 12);
					if (diff != 0) {
						// resize so that the batch size isn't off
						pipe3lines.resize(pipe3lines.size() + PIPE3_LINE_BATCH_LIMIT*12-diff, 0);
					}
					// set the lines that aren't drawn in the batch to zero
					int offset = (drawnLines + lineCount) * 12;
					int size = 12 * (PIPE3_LINE_BATCH_LIMIT - lineCount);
					memset(pipe3lines.data() + offset, 0, size * sizeof(float));
				}
				pipe3lineVB.setData(PIPE3_LINE_BATCH_LIMIT * 12 * sizeof(float), pipe3lines.data() + drawnLines * 12);
				pipe3lineVA.drawLines(&pipe3lineIB);
				drawnLines += PIPE3_LINE_BATCH_LIMIT;
			}
			pipe3lines.clear();
		}

		EnableBlend();

		if (!uiObjects.empty()) {
			if (info.window != m_parent)
				log::out << log::YELLOW << "Renderer and active context is different!\n";
		}
		// setup
		ShaderAsset* guiShad = m_parent->getStorage()->get<ShaderAsset>("gui");
		if (!guiShad) return;

		guiShad->bind();
		guiShad->setVec2("uWindow", { GetWidth(), GetHeight() });

		ShaderAsset* pipeShad = m_parent->getStorage()->get<ShaderAsset>("uiPipeline");
		if (!pipeShad) return;

		pipeShad->bind();
		pipeShad->setVec2("uWindow", { GetWidth(), GetHeight() });
		// needs to be done once
		for (int i = 0; i < 8; i++) {
			pipeShad->setInt("uSampler[" + std::to_string(i) + std::string("]"), i);
		}

		ShaderAsset* objectShad = m_parent->getStorage()->get<ShaderAsset>("object");
		if (!objectShad) return;

		objectShad->bind();
		setOrthogonal();
		updateProjection(objectShad, glm::mat4(1));
		shader->setVec3("uCamera", camera.getPosition());

		ShaderAsset* armatureShad = m_parent->getStorage()->get<ShaderAsset>("armature");
		if (!armatureShad) return;

		armatureShad->bind();
		setOrthogonal();
		updateProjection(armatureShad, glm::mat4(1));
		shader->setVec3("uCamera", camera.getPosition());

	
		// Method one
		int floatIndex = 0;
		std::unordered_map<Texture*, int> boundTextures;
		char lastShader = 0;
		while (true) {
			char type = uiObjects.readType();
			if (type == 'B') {
				ui::Box* box = uiObjects.readItem<ui::Box>();

				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 0] = { box->x, box->y, 0, 0, box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, -1 };
				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 1] = { box->x, box->y + box->h, 0, 0, box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, -1 };
				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 2] = { box->x + box->w, box->y + box->h, 0, 0, box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, -1 };
				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 3] = { box->x + box->w, box->y, 0, 0, box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, -1 };

				floatIndex++;

			} else if (type == 'T') {
				ui::TexturedBox* box = uiObjects.readItem<ui::TexturedBox>();

				int slot = boundTextures.size();
				if (boundTextures.count(box->texture)) {
					boundTextures[box->texture] = boundTextures.size();
				} else {
					slot = boundTextures[box->texture];
				}

				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 0] = { box->x,			box->y,			box->u,			box->v + box->vh,				box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, (float)slot };
				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 1] = { box->x,			box->y + box->h,	box->u,			box->v,				box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, (float)slot };
				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 2] = { box->x + box->w, box->y + box->h,	box->u + box->uw,	box->v,				box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, (float)slot };
				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 3] = { box->x + box->w, box->y,			box->u + box->uw,	box->v + box->vh,				box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, (float)slot };

				floatIndex++;

			} else if (type == 'S') {
				// Is done below
			} else if (type == 'M') {

			}

			if (floatIndex == MAX_BOX_BATCH || boundTextures.size() >= 7 || (type == 0 && floatIndex != 0)||((type=='S'||type=='M') && lastShader != 'P' && lastShader != 0)) {

				for (auto [texture, index] : boundTextures) {
					texture->bind(index);
				}

				if (floatIndex != MAX_BOX_BATCH)
					ZeroMemory(floatArray + floatIndex * 4 * VERTEX_SIZE, (MAX_BOX_BATCH - floatIndex) * 4 * VERTEX_SIZE * sizeof(float));

				if (lastShader != 'P')
					pipeShad->bind();
				lastShader = 'P';

				boxVBO.setData(MAX_BOX_BATCH * 4 * VERTEX_SIZE * sizeof(float), floatArray);
				boxVAO.draw(&boxIBO);

				floatIndex = 0;
				boundTextures.clear();
			}
			if (type == 'S') {
				ui::PipeTextBox* box = uiObjects.readItem<ui::PipeTextBox>();

				if (lastShader != 'G')
					guiShad->bind();
				lastShader = 'G';
				guiShad->setVec2("uPos", { box->x, box->y });
				guiShad->setVec2("uSize", { 1, 1 });
				guiShad->setVec4("uColor", box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a);

				//DrawString(box->font, uiStrings[box->text_index], false, box->h, 9999, 9999, box->at);
				DrawString(box->font, uiStrings[box->text_index], box->h, box->at);

				// don't continue with other stuff
				continue;
			}
			if (type == 'M') {
				ui::ModelBox* box = uiObjects.readItem<ui::ModelBox>();
				
				lastShader = 'O';
				
				ModelAsset* modelAsset = m_parent->getStorage()->get<ModelAsset>(uiStrings[box->model_text_index]);
				glm::mat4& modelMatrix = box->matrix;
				//glm::mat4 modelMatrix = ToMatrix(obj->rigidBody->getTransform());

				if (!modelAsset) return;
				if (!modelAsset->valid()) return;

				// Get individual transforms
				std::vector<glm::mat4> transforms = modelAsset->getParentTransforms(nullptr);

				std::unordered_map<MeshAsset*, std::vector<glm::mat4>> normalObjects;
				// get final matrices for instances
				for (uint32_t i = 0; i < modelAsset->instances.size(); ++i) {
					AssetInstance& instance = modelAsset->instances[i];
					//log::out << " " << instance.asset->filePath<< " " << (int)instance.asset->type << " "<< (int)asset->meshType << "\n";
					if (instance.asset->type == MeshAsset::TYPE) {
						MeshAsset* asset = (MeshAsset*)instance.asset;

						if (asset->meshType == MeshAsset::MeshType::Normal) {
							glm::mat4 out;

							out = modelMatrix * transforms[i] * instance.localMat;

							if (normalObjects.count(asset) > 0) {
								normalObjects[asset].push_back(out);
							} else {
								normalObjects[asset] = std::vector<glm::mat4>();
								normalObjects[asset].push_back(out);
							}
						}
					}
				}

				shader = m_parent->getStorage()->get<ShaderAsset>("object");
				if (!shader->getError()) {
					shader->bind();

					//bindLights(shader, { 0,0,0 });
					shader->setMat4("uTransform", glm::mat4(0)); // zero in mat4 means we do instanced rendering. uTransform should be ignored
					for (auto& [asset, vector] : normalObjects) {
						for (uint32_t j = 0; j < asset->materials.size(); ++j) {// Maximum of 4 materials
							asset->materials[j]->bind(shader, j);
						}
						uint32_t remaining = vector.size();
						while (remaining > 0) {
							uint32_t batchAmount = std::min(remaining, Renderer::INSTANCE_BATCH);

							getInstanceBuffer().setData(batchAmount * sizeof(glm::mat4), (vector.data() + vector.size() - remaining));

							asset->vertexArray.selectBuffer(3, &getInstanceBuffer());
							asset->vertexArray.draw(&asset->indexBuffer, batchAmount);
							remaining -= batchAmount;
						}
					}
					normalObjects.clear();
				}
				shader = m_parent->getStorage()->get<ShaderAsset>("armature");
				lastShader = 'A';
				if (!shader->getError()) {
					shader->bind();
						
					//bindLights(shader, modelMatrix[3]);
					//-- Draw instances
					AssetInstance* armatureInstance = nullptr;
					ArmatureAsset* armatureAsset = nullptr;
					if (transforms.size() == modelAsset->instances.size()) {
						for (uint32_t i = 0; i < modelAsset->instances.size(); ++i) {
							AssetInstance& instance = modelAsset->instances[i];

							if (instance.asset->type == AssetArmature) {
								armatureInstance = &instance;
								armatureAsset = (ArmatureAsset*)instance.asset;
							} else if (instance.asset->type == AssetMesh) {
								if (!armatureInstance)
									continue;

								MeshAsset* meshAsset = (MeshAsset*)instance.asset;
								if (meshAsset->meshType == MeshAsset::MeshType::Boned) {
									if (instance.parent == -1)
										continue;

									std::vector<glm::mat4> boneMats;
									std::vector<glm::mat4> boneTransforms = modelAsset->getArmatureTransforms(nullptr, transforms[i], armatureInstance, armatureAsset, &boneMats);

									for (uint32_t j = 0; j < boneTransforms.size(); ++j) {
										shader->setMat4("uBoneTransforms[" + std::to_string(j) + "]", boneTransforms[j] * instance.localMat);
									}

									for (uint32_t j = 0; j < meshAsset->materials.size(); ++j) {// Maximum of 4 materials
										meshAsset->materials[j]->bind(shader, j);
									}
									shader->setMat4("uTransform", modelMatrix * transforms[i]);
									meshAsset->vertexArray.draw(&meshAsset->indexBuffer);
								}
								}
							}
					}
				}

				continue;
			}
			lastShader = 1;
			if (type == 0)
				break;
		}
		uiObjects.clear();
		uiStrings.clear();

		//std::unordered_map<MeshAsset*, std::vector<glm::mat4>> normalObjects;

		//shader = m_parent->getStorage()->get<ShaderAsset>("object");
		//if (!shader->getError()) {
		//	shader->bind();
		//	shader->setVec3("uCamera", camera.getPosition());
		//	for (int i = 0; i < modelObjects.size(); i++) {
		//		ModelAsset* modelAsset = modelObjects[i].modelAsset;
		//		glm::mat4& modelMatrix = modelObjects[i].matrix;
		//		//glm::mat4 modelMatrix = ToMatrix(obj->rigidBody->getTransform());

		//		if (!modelAsset) return;
		//		if (!modelAsset->valid()) return;

		//		//Animator* animator = &obj->animator;

		//		// Get individual transforms
		//		std::vector<glm::mat4> transforms = modelAsset->getParentTransforms(nullptr);
		//		/*
		//		if ((transform->position.x-camPos.x)* (transform->position.x - camPos.x)
		//			+(transform->position.y - camPos.y)*(transform->position.y - camPos.y)+
		//			(transform->position.z - camPos.z)*(transform->position.z - camPos.z)>200*200) {
		//			continue;
		//		}
		//		*/
		//		// Draw instances
		//		for (uint32_t i = 0; i < modelAsset->instances.size(); ++i) {
		//			AssetInstance& instance = modelAsset->instances[i];
		//			//log::out << " " << instance.asset->filePath<< " " << (int)instance.asset->type << " "<< (int)asset->meshType << "\n";
		//			if (instance.asset->type == MeshAsset::TYPE) {
		//				MeshAsset* asset = (MeshAsset*)instance.asset;

		//				if (asset->meshType == MeshAsset::MeshType::Normal) {
		//					glm::mat4 out;

		//					//if (animator->asset)
		//					out = modelMatrix * transforms[i] * instance.localMat;
		//					//else
		//						//out = modelMatrix * instance.localMat;

		//					if (normalObjects.count(asset) > 0) {
		//						normalObjects[asset].push_back(out);
		//					} else {
		//						normalObjects[asset] = std::vector<glm::mat4>();
		//						normalObjects[asset].push_back(out);
		//					}
		//				}
		//			}
		//		}

		//		if (modelObjects[i].isOrthogonal) {
		//			setOrthogonal();
		//			updateProjection(shader, glm::mat4(1));
		//		} else {
		//			setProjection();
		//			updateProjection(shader);
		//		}
		//		//bindLights(shader, { 0,0,0 });
		//		shader->setMat4("uTransform", glm::mat4(0)); // zero in mat4 means we do instanced rendering. uTransform should be ignored
		//		for (auto& [asset, vector] : normalObjects) {
		//			//if (asset->meshType == MeshAsset::MeshType::Normal) {
		//			for (uint32_t j = 0; j < asset->materials.size(); ++j) {// Maximum of 4 materials
		//				asset->materials[j]->bind(shader, j);
		//			}
		//			//}
		//			uint32_t remaining = vector.size();
		//			while (remaining > 0) {
		//				uint32_t batchAmount = std::min(remaining, Renderer::INSTANCE_BATCH);

		//				getInstanceBuffer().setData(batchAmount * sizeof(glm::mat4), (vector.data() + vector.size() - remaining));

		//				asset->vertexArray.selectBuffer(3, &getInstanceBuffer());
		//				asset->vertexArray.draw(&asset->indexBuffer, batchAmount);
		//				remaining -= batchAmount;
		//			}
		//		}
		//		normalObjects.clear();
		//	}
		//}
		//shader = m_parent->getStorage()->get<ShaderAsset>("armature");
		//if (!shader->getError()) {
		//	shader->bind();
		//	//renderer->updateProjection(shader);
		//	//shader->setVec3("uCamera", camera->getPosition());

		//	//EngineObjectIterator iterator = world->getIterator();
		//	//EngineObject* obj;
		//	//while (obj = iterator.next()) {
		//		//for (int oi = 0; oi < objects.size(); ++oi) {
		//			//EngineObject* obj = objects[oi];
		//	for (int i = 0; i < modelObjects.size(); i++) {
		//		ModelAsset* modelAsset = modelObjects[i].modelAsset;

		//		if (!modelAsset) continue;
		//		if (!modelAsset->valid()) continue;


		//		glm::mat4 modelMatrix = modelObjects[i].matrix;
		//		//ToMatrix(obj->rigidBody->getTransform());

		//		//Animator* animator = &obj->animator;

		//		if (modelObjects[i].isOrthogonal) {
		//			setOrthogonal();
		//			updateProjection(shader,glm::mat4(1)); // <- Hello, change this ortho thing because it's easy to forget the glm::mat4(1) argument as the viewMatrix. And yes, this comment should be put in a better spot.
		//		} else {
		//			setProjection();
		//			updateProjection(shader);
		//		}

		//		//bindLights(shader, modelMatrix[3]);

		//		// Get individual transforms
		//		std::vector<glm::mat4> transforms;
		//		transforms = modelAsset->getParentTransforms(nullptr);

		//		//-- Draw instances
		//		AssetInstance* armatureInstance = nullptr;
		//		ArmatureAsset* armatureAsset = nullptr;
		//		if (transforms.size() == modelAsset->instances.size()) {
		//			for (uint32_t i = 0; i < modelAsset->instances.size(); ++i) {
		//				AssetInstance& instance = modelAsset->instances[i];

		//				if (instance.asset->type == AssetArmature) {
		//					armatureInstance = &instance;
		//					armatureAsset = (ArmatureAsset*)instance.asset;
		//				} else if (instance.asset->type == AssetMesh) {
		//					if (!armatureInstance)
		//						continue;

		//					MeshAsset* meshAsset = (MeshAsset*)instance.asset;
		//					if (meshAsset->meshType == MeshAsset::MeshType::Boned) {
		//						if (instance.parent == -1)
		//							continue;

		//						std::vector<glm::mat4> boneMats;
		//						std::vector<glm::mat4> boneTransforms = modelAsset->getArmatureTransforms(nullptr, transforms[i], armatureInstance, armatureAsset, &boneMats);

		//						//-- For rendering bones
		//						//glm::mat4 basePos = modelMatrix* transforms[i];
		//						//glm::mat4 boneLast;
		//						//// one bone is missing. the first bone goes from origin to origin which makes it seem like a bone is missing
		//						//for (int j = 0; j < boneMats.size(); j++) {
		//						//	glm::mat4 boneMat = basePos*boneMats[j];
		//						//	if(j!=0) // skip first boneLast 
		//						//		info.window->getRenderer()->DrawLine(boneLast[3], boneMat[3]);
		//						//	boneLast = boneMat;
		//						//}

		//						for (uint32_t j = 0; j < boneTransforms.size(); ++j) {
		//							shader->setMat4("uBoneTransforms[" + std::to_string(j) + "]", boneTransforms[j] * instance.localMat);
		//						}

		//						for (uint32_t j = 0; j < meshAsset->materials.size(); ++j) {// Maximum of 4 materials
		//							meshAsset->materials[j]->bind(shader, j);
		//						}
		//						shader->setMat4("uTransform", modelMatrix * transforms[i]);
		//						meshAsset->vertexArray.draw(&meshAsset->indexBuffer);
		//					}
		//				}
		//			}
		//		}
		//	}
		//}
		//modelObjects.clear();
	}
}
#endif