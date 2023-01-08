#include "Engone/Rendering/CommonRenderer.h"

#include "Engone/Window.h"
#include "Engone/Application.h"
#include "Engone/Engone.h"

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
static const char* particleGLSL = {
#include "Engone/Shaders/particle.glsl"
};

#ifndef PIPE3_LINE_BATCH_LIMIT
#define PIPE3_LINE_BATCH_LIMIT 500
#endif
#ifndef PIPE3_LINE_RESERVE
#define PIPE3_LINE_RESERVE PIPE3_LINE_BATCH_LIMIT
#endif

namespace engone {
	void CommonRenderer::init() {
		if (m_initialized) return;
		m_initialized = true;
		setActiveRenderer();
		Window* win = engone::GetActiveWindow();

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
		textIBO.setData(6 * TEXT_BATCH * sizeof(float), indes);
		textVBO.setData(16 * TEXT_BATCH * sizeof(float), nullptr);
		textVAO.addAttribute(4, &textVBO);

		AssetStorage* assets = win->getStorage();

		// pipeline

		assets->set<ShaderAsset>("uiPipeline", ALLOC_NEW(ShaderAsset)(uiPipelineGLSL));

		float near_plane = 1.f, far_plane = 20.5f;
		lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);

		// ISSUE: how to deal with tracking of assets, when window is destroyed, free assets.
		//	What if you have added an asset twice with different names
		//  Freeing assets not allocate with tracker would give negative memoray allocations.
		//  Either the user needs to define some flag?
		//  Maybe track with addMemory event if assets are added twice it would be fine?
		//  maybe use tracker inside constructors, You can't have inside constructors because a stack allocated class would be tracked.

		// Line pipeline
		assets->set<ShaderAsset>("lines3d", ALLOC_NEW(ShaderAsset)(linesGLSL));
		pipe3lines.reserve(PIPE3_LINE_RESERVE * 12);
		pipe3lineVB.setData(PIPE3_LINE_BATCH_LIMIT * 12 * sizeof(float), nullptr);
		uint32_t indLine[PIPE3_LINE_BATCH_LIMIT * 2];
		for (int i = 0; i < PIPE3_LINE_BATCH_LIMIT * 2; i++) {
			indLine[i] = i;
		}
		pipe3lineIB.setData(PIPE3_LINE_BATCH_LIMIT * 2 * sizeof(float), indLine);
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
		cubeVBO.setData(48 * sizeof(float), cubeVertices);
		uint32_t cubeIndices[36]{
			0, 2, 1, 2, 3, 1,
			0, 1, 4, 1, 5, 4,
			0, 4, 2, 4, 6, 2,
			2, 6, 3, 6, 7, 3,
			1, 3, 5, 3, 7, 5,
			4, 5, 6, 5, 7, 6,
		};
		cubeIBO.setData(36 * sizeof(uint32_t), cubeIndices);

		cubeInstanceVBO.setData(MAX_CUBE_BATCH * sizeof(Cube), nullptr);
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
		quadVA.addAttribute(4, &quadVB);

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
	}
	void CommonRenderer::drawQuad(LoopInfo& info) {
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
	Camera* CommonRenderer::getCamera() {
		return &camera;
	}
	glm::mat4& CommonRenderer::getLightProj() {
		return lightProjection;
	}
	void CommonRenderer::setPerspective() {
		float ratio = GetWidth() / GetHeight();
		if (isfinite(ratio))
			perspectiveMatrix = glm::perspective(fov, ratio, zNear, zFar);
	}
	void CommonRenderer::setOrthogonal() {
		//float ratio = GetWidth() / GetHeight();
		//projMatrix = glm::ortho(-ratio, ratio, -1.f, 1.f, zNear, zFar);
		// you need to apply the ratio to the models yourself
		orthogonalMatrix = glm::ortho(-1.f, 1.f, -1.f, 1.f, zNear, zFar);
	}
	glm::mat4& CommonRenderer::getPerspective() {
		setPerspective(); // ensure the matrix is up to date
		return perspectiveMatrix;
	}
	glm::mat4& CommonRenderer::getOrthogonal() {
		setOrthogonal(); // ensure the matrix is up to date
		return orthogonalMatrix;
	}
	// Should be done once in the render loop, currently done in Engone::render
	//void Renderer::updateViewMatrix(double lag) {
	//	viewMatrix = glm::inverse(
	//		glm::translate(glm::mat4(1.0f), getCamera()->position) *
	//		glm::rotate(getCamera()->rotation.y, glm::vec3(0, 1, 0)) *
	//		glm::rotate(getCamera()->rotation.x, glm::vec3(1, 0, 0))
	//	);
	//}
	void CommonRenderer::updatePerspective(Shader* shader) {
		setPerspective();
		if (shader != nullptr)
			shader->setMat4("uProj", perspectiveMatrix * camera.getViewMatrix());
		else
			log::out << log::RED << "CommonRenderer::updatePerspective : shader is null";
	}
	void CommonRenderer::updatePerspective(Shader* shader, glm::mat4 viewMatrix) {
		setPerspective();
		if (shader != nullptr)
			shader->setMat4("uProj", perspectiveMatrix * viewMatrix);
		else
			log::out << log::RED << "CommonRenderer::updatePerspective : shader is null";
	}
	void CommonRenderer::updateOrthogonal(Shader* shader) {
		setOrthogonal();
		if (shader != nullptr)
			shader->setMat4("uProj", orthogonalMatrix * camera.getViewMatrix());
		else
			log::out << log::RED << "CommonRenderer::updateOrthogonal : shader is null";
	}
	void CommonRenderer::updateOrthogonal(Shader* shader, glm::mat4 viewMatrix) {
		setOrthogonal();
		if (shader != nullptr)
			shader->setMat4("uProj", orthogonalMatrix * viewMatrix);
		else
			log::out << log::RED << "CommonRenderer::updateOrthogonal : shader is null";
	}
	static void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3) {
		ar[ind] = f0;
		ar[ind + 1] = f1;
		ar[ind + 2] = f2;
		ar[ind + 3] = f3;
	}
	void CommonRenderer::drawString(FontAsset* font, const std::string& text, bool center, float wantedHeight, float maxWidth, float maxHeight, int atChar) {
		if (text.length() == 0 && atChar == -1)
			return;
		if (font == nullptr) {
			std::cout << "DrawString: Font is null\n";
			return;
		} else if (font->getError()) {
			return;
		} else {
			Window* win = engone::GetActiveWindow();
			//Shader* guiShader = m_parent->getAssets()->get<Shader>("gui");
			ShaderAsset* guiShader = win->getStorage()->get<ShaderAsset>("gui");
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
					textVBO.setData(16 * TEXT_BATCH * sizeof(float), verts);
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
		memset(verts + 16 * charIndex, 0, 16 * (TEXT_BATCH - charIndex) * sizeof(float));

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
	void CommonRenderer::drawString(FontAsset* font, const std::string& text, float height, int cursorPosition) {
		//log::out << text.length() << " " << cursorPosition << "\n";

		if (text.length() == 0 && cursorPosition == -1)
			return;
		if (font == nullptr) {
			std::cout << "DrawString: Font is null\n";
			return;
		} else if (font->getError()) {
			return;
		} else {
			Window* win = engone::GetActiveWindow();
			//Shader* guiShader = m_parent->getAssets()->get<Shader>("gui");
			ShaderAsset* guiShader = win->getStorage()->get<ShaderAsset>("gui");
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
				log::out << log::YELLOW << "Renderer::DrawString - Bad code when drawing text\n";
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
		for (uint32_t j = 0; j < text.size(); ++j) {
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
	void CommonRenderer::drawCube(glm::mat4 matrix, glm::vec3 scale, glm::vec3 color) {
		cubeObjects.push_back({ glm::scale(matrix,scale),color });
	}
	void CommonRenderer::drawCubeRaw() {
		cube2VAO.draw(&cube2IBO);
	}
	void CommonRenderer::drawNetCube(glm::mat4 matrix, glm::vec3 scale, glm::vec3 color) {
		auto p = [&](float x, float y, float z) {
			return (matrix * glm::translate(glm::vec3(scale.x * (x - .5), scale.y * (y - .5), scale.z * (z - .5))))[3];
		};
		drawLine(p(0, 0, 0), p(1, 0, 0), color);
		drawLine(p(0, 0, 0), p(0, 0, 1), color);
		drawLine(p(1, 0, 1), p(1, 0, 0), color);
		drawLine(p(1, 0, 1), p(0, 0, 1), color);

		drawLine(p(0, 0, 0), p(0, 1, 0), color);
		drawLine(p(1, 0, 0), p(1, 1, 0), color);
		drawLine(p(1, 0, 1), p(1, 1, 1), color);
		drawLine(p(0, 0, 1), p(0, 1, 1), color);

		drawLine(p(0, 1, 0), p(1, 1, 0), color);
		drawLine(p(0, 1, 0), p(0, 1, 1), color);
		drawLine(p(1, 1, 1), p(1, 1, 0), color);
		drawLine(p(1, 1, 1), p(0, 1, 1), color);
	}
	constexpr float pis() {
		return glm::pi<float>();
	}
	void CommonRenderer::drawSphere(glm::vec3 position, float radius, glm::vec3 color) {
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
	void CommonRenderer::drawLine(glm::vec3 a, glm::vec3 b, glm::vec3 rgb) {
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
	void CommonRenderer::drawTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
		drawLine(a, b);
		drawLine(b, c);
		drawLine(c, a);
	}
	static int min(int a, int b) {
		return a < b ? a : b;
	}
	void CommonRenderer::render(LoopInfo& info) {
		EnableDepth();
		Window* win = engone::GetActiveWindow();

		//if (!instanceBuffer.initialized()) {
		//	instanceBuffer.setData(INSTANCE_BATCH * sizeof(glm::mat4), nullptr);
		//}

		ShaderAsset* shader = win->getStorage()->get<ShaderAsset>("renderer");
		if (shader) {
			shader->bind();
			updatePerspective(shader);
			shader->setVec3("uCamera", camera.getPosition());
			uint32_t drawnCubes = 0;
			uint32_t cubeCount = 0;

			win->getParent()->getEngine()->bindLights(shader, { 0,0,0 });

			while (drawnCubes < cubeObjects.size()) {
				cubeCount = min(MAX_CUBE_BATCH, cubeObjects.size() - drawnCubes);

				// Cube only consists of floats which is important. may want to zero memory the last bit of the instance buffer.
				cubeInstanceVBO.setData(cubeCount * sizeof(Cube), (cubeObjects.data() + drawnCubes));
				cubeVAO.selectBuffer(2, &cubeInstanceVBO);
				cubeVAO.draw(&cubeIBO, cubeCount);
				drawnCubes += cubeCount;
			}
			cubeObjects.clear();
		}
		shader = win->getStorage()->get<ShaderAsset>("lines3d");
		if (shader) {
			shader->bind();
			updatePerspective(shader);
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
						pipe3lines.resize(pipe3lines.size() + PIPE3_LINE_BATCH_LIMIT * 12 - diff, 0);
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
	}
	static CommonRenderer* s_activeRenderer = nullptr;
	void CommonRenderer::setActiveRenderer() {
		s_activeRenderer = this;
	}
	CommonRenderer* GetActiveRenderer() {
		return s_activeRenderer;
	}
	CommonRenderer::~CommonRenderer() {
		if (s_activeRenderer == this)
			s_activeRenderer == nullptr;
	}
}