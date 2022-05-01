
#include "Renderer.h"
#include "../Handlers/AssetHandler.h"

#include "../Window.h"

#include "../EventHandler.h"

static const char* uiPipelineGLSL = {
#include "../Shaders/uiPipeline.glsl"
};
static const char* rendererGLSL = {
#include "../Shaders/renderer.glsl"
};

namespace engone {

	//-- world

	glm::mat4 lightProjection;
	static glm::mat4 projMatrix;
	static glm::mat4 viewMatrix;
	static float fov, zNear, zFar;

	//static const int colliderVertexLimit = 400*3, colliderIndexLimit = 600*2;
#ifndef RAW_VERTEX_LIMIT
#define RAW_VERTEX_LIMIT 400*3
#endif
#ifndef RAW_INDEX_LIMIT
#define RAW_INDEX_LIMIT 600*2
#endif
	//static LineBuffer colliderBuffer;
	static VertexBuffer rawVBO;
	static IndexBuffer rawIBO;
	static VertexArray rawVAO;
	static float rawVertices[RAW_VERTEX_LIMIT];
	static uint32_t rawIndices[RAW_INDEX_LIMIT];

#ifndef LINE_LIMIT
#define LINE_LIMIT 100
#endif
	//static const int lineBufferLimit = 100;
	//static LineBuffer lineBuffer;
	static VertexBuffer lineVBO;
	static IndexBuffer lineIBO;
	static VertexArray lineVAO;
	static float lineVertices[LINE_LIMIT * 6];
	static uint32_t lineIndices[LINE_LIMIT * 2];

#ifndef MAX_CUBE_BATCH
#define MAX_CUBE_BATCH 100
#endif
#ifndef CUBE_INSTANCE_SIZE
#define CUBE_INSTANCE_SIZE 16+3
#endif
	//static const int MAX_CUBE_BATCH = 100;
	//static const int CUBE_INSTANCE_SIZE = 4*4+3;
	static VertexArray cubeVAO;
	static VertexBuffer cubeVBO;
	static VertexBuffer cubeInstanceVBO;
	static IndexBuffer cubeIBO;

	static std::vector<Cube> cubeObjects;
	static std::vector<Sphere> sphereObjects;

	//-- ui

	static const int TEXT_BATCH = 40;
	static float verts[4 * 4 * TEXT_BATCH];
	static VertexBuffer textVBO;
	static VertexArray textVAO;
	static IndexBuffer textIBO;
	//static TriangleBuffer textBuffer, rectBuffer;

	static const int MAX_BOX_BATCH = 100;
	static const int VERTEX_SIZE = 2 + 2 + 4 + 1;
	static VertexArray boxVAO;
	static VertexBuffer boxVBO;
	static IndexBuffer boxIBO;
	static float floatArray[4 * VERTEX_SIZE * MAX_BOX_BATCH];

	static ItemVector uiObjects;
	static std::vector<std::string> uiStrings;

	//-- funcs

	void InitRenderer(EngoneHint hints) {
		//if (hints == EngoneHint::UI) {
			uint32_t indes[TEXT_BATCH * 6];
			for (int i = 0; i < TEXT_BATCH; i++) {
				indes[0 + 6 * i] = 0 + 4 * i;
				indes[1 + 6 * i] = 1 + 4 * i;
				indes[2 + 6 * i] = 2 + 4 * i;
				indes[3 + 6 * i] = 2 + 4 * i;
				indes[4 + 6 * i] = 3 + 4 * i;
				indes[5 + 6 * i] = 0 + 4 * i;
			}

			textVBO.setData(4 * 4 * TEXT_BATCH, nullptr);
			textIBO.setData(6 * TEXT_BATCH, indes);
			textVAO.addAttribute(4, &textVBO);

			//textBuffer.Init(true, nullptr, 4 * 4 * TEXT_BATCH, indes, 6 * TEXT_BATCH);
			//textBuffer.SetAttrib(0, 4, 4, 0);
			// 
			//float temp[]{ // This will be updated in when using DrawRect or DrawUVRect
			//	0, 0, 0, 0,
			//	0, 1, 0, 1,
			//	1, 1, 1, 1,
			//	1, 0, 1, 0,
			//};
			//unsigned int temp2[]{
			//	0,1,2,
			//	2,3,0
			//};
			//rectBuffer.Init(true, temp, 16, temp2, 6);
			//rectBuffer.SetAttrib(0, 4, 4, 0);

			// pipeline
			AddAsset<Shader>("uiPipeline", new Shader(uiPipelineGLSL));

			boxVBO.setData(4 * VERTEX_SIZE * MAX_BOX_BATCH, nullptr);

			uint32_t intArray[6 * MAX_BOX_BATCH];
			for (int i = 0; i < MAX_BOX_BATCH; i++) {
				intArray[i * 6] = i * 4 + 0;
				intArray[i * 6 + 1] = i * 4 + 1;
				intArray[i * 6 + 2] = i * 4 + 2;
				intArray[i * 6 + 3] = i * 4 + 2;
				intArray[i * 6 + 4] = i * 4 + 3;
				intArray[i * 6 + 5] = i * 4 + 0;
			}
			boxIBO.setData(6 * MAX_BOX_BATCH, intArray);

			boxVAO.addAttribute(2); // pos
			boxVAO.addAttribute(2); // uv
			boxVAO.addAttribute(4); // color
			boxVAO.addAttribute(1, &boxVBO); // texture
		//}
		//if (hints == EngoneHint::Game3D) {

			float near_plane = 1.f, far_plane = 20.5f;
			lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);

			fov = 90.f;
			zNear = 0.1f;
			zFar = 400.f;
			SetProjection(GetWidth() / GetHeight());

			//colliderVertices = new float[colliderVertexLimit];// {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1}; not using color
			//colliderIndices = new unsigned int[colliderIndexLimit];// {0, 1, 0, 2, 0, 3};

			ZeroMemory(rawVertices, RAW_VERTEX_LIMIT * sizeof(float));
			ZeroMemory(rawIndices, RAW_INDEX_LIMIT * sizeof(uint32_t));

			rawVBO.setData(RAW_VERTEX_LIMIT, rawVertices);
			rawIBO.setData(RAW_INDEX_LIMIT, rawIndices);
			rawVAO.addAttribute(3, &rawVBO);
			//colliderBuffer.Init(true, colliderVertices, colliderVertexLimit, colliderIndices, colliderIndexLimit);
			//colliderBuffer.SetAttrib(0, 3, 3, 0);

			/*lineVertices = new float[lineBufferLimit * 6];
			lineIndices = new unsigned int[lineBufferLimit * 2];*/

			ZeroMemory(lineVertices, sizeof(float) * LINE_LIMIT * 6);
			for (int i = 0; i < LINE_LIMIT * 2; i++) {
				lineIndices[i] = i;
			}

			//lineBuffer.Init(true, nullptr, LINE_LIMIT * 6, lineIndices, LINE_LIMIT * 2);
			//lineBuffer.SetAttrib(0, 3, 3, 0);
			lineVBO.setData(LINE_LIMIT * 6, nullptr);
			lineIBO.setData(LINE_LIMIT * 2, lineIndices);
			lineVAO.addAttribute(3, &lineVBO);

			AddAsset<Shader>("renderer", new Shader(rendererGLSL));
			float v[48]{
				0,0,0, -1,0,0,
				1,0,0, 0,-1,0,
				0,1,0, 0,0,-1,
				1,1,0, 0,0,0,

				0,0,1, 0,0,0,
				1,0,1, 1,0,0,
				0,1,1, 0,1,0,
				1,1,1, 0,0,1,
			};
			for (int i = 0; i < 24; i++)
				v[i % 3 + (i / 3) * 6] -= 0.5;
			uint32_t i[36]{
				0, 2, 1, 1, 2, 3,
				0, 1, 4, 1, 5, 4,
				0, 4, 2, 2, 4, 6,
				2, 6, 3, 3, 6, 7,
				1, 3, 5, 3, 7, 5,
				4, 5, 6, 5, 7, 6,
			};
			cubeVBO.setData(48, v);
			cubeInstanceVBO.setData(MAX_BOX_BATCH * CUBE_INSTANCE_SIZE, nullptr);
			cubeIBO.setData(36, i);
			cubeVAO.addAttribute(3);
			cubeVAO.addAttribute(3, &cubeVBO);
			cubeVAO.addAttribute(4, 1);
			cubeVAO.addAttribute(4, 1);
			cubeVAO.addAttribute(4, 1);
			cubeVAO.addAttribute(4, 1);
			cubeVAO.addAttribute(3, 1, &cubeInstanceVBO);
		//}
	}
	void UninitRenderer() {
		//delete[] colliderVertices;
		//delete[] colliderIndices;
		//delete[] lineVertices;
		//delete[] lineIndices;

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
	static Camera engine_camera;
	Camera* GetCamera() {
		return &engine_camera;
	}
	glm::mat4& GetLightProj() {
		return lightProjection;
	}
	void SetProjection(float ratio) {
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
	}
	void RenderRenderer() {
		EnableDepth();
		Shader* shad = GetAsset<Shader>("renderer");
		if (shad) {
			shad->bind();
			UpdateProjection(shad);
			int drawnCubes = 0;
			int cubeCount = 0;

			while (drawnCubes < cubeObjects.size()) {
				cubeCount = min(MAX_BOX_BATCH, cubeObjects.size() - drawnCubes);

				cubeInstanceVBO.setData(cubeCount * sizeof(Cube), (float*)(cubeObjects.data() + drawnCubes));
				cubeVAO.selectBuffer(2, &cubeInstanceVBO);
				cubeVAO.draw(&cubeIBO, cubeCount);
				drawnCubes += cubeCount;
			}
			cubeObjects.clear();
		}
	}
	static void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3) {
		ar[ind] = f0;
		ar[ind + 1] = f1;
		ar[ind + 2] = f2;
		ar[ind + 3] = f3;
	}
	void DrawString(Font* font, const std::string& text, bool center, float wantedHeight, float maxWidth, float maxHeight, int atChar) {
		if (text.length() == 0 && atChar == -1)
			return;
		if (font == nullptr) {
			std::cout << "DrawString: Font is null\n";
			return;
		} else if (font->texture.error) {
			return;
		} else {
			Shader* guiShader = GetAsset<Shader>("gui");
			if (guiShader != nullptr)
				guiShader->setInt("uColorMode", 1);
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
		for (int i = 0; i < text.length(); i++) {
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
			for (int i = 0; i < lineWidths.size(); i++) {
				//lineWidths[i] -= spacing * (lines[i].length() - 1);
				lineWidths[i] *= maxWidth / reachedWidth;
				//lineWidths[i] += spacing*(lines[i].length()-1);
				//std::cout << rowWidths[i] << "\n";
			}
		}
		if (lineWidths.size() * wantedHeight > maxHeight) {
			for (int i = 0; i < lineWidths.size(); i++) {
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
		int dataIndex = 0;
		//std::cout << lines[0] << "\n";
		if (atChar != -1) { // do marker
			//std::cout << lines.size()<<"\n";
			for (int i = 0; i < lines.size(); i++) {
				if (center)
					x = (maxWidth - lineWidths[i]) / 2;
				else
					x = 0;
				for (int j = 0; j < lines[i].length(); j++) {
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
		for (int i = 0; i < lines.size(); i++) {
			//std::cout << "[" << lines[i] << "] " << "\n";
			float x = 0;
			if (center)
				x = (maxWidth - lineWidths[i]) / 2;
			//std::cout << lines[i].length()<<"\n";
			for (int j = 0; j < lines[i].length(); j++) {
				char chr = lines[i][j];
				//if (chr == '\n')
				//	continue;
				float wStride = wantedHeight * (font->charWid[chr] / (float)font->charSize);

				float wuv = font->charWid[chr] / (float)font->imgSize;
				float u = (chr % 16);
				float v = 15 - (chr / 16);

				Insert4(verts, 16 * dataIndex, x, y, (u) / 16, (v + 1) / 16);
				Insert4(verts, 4 + 16 * dataIndex, x, y + wantedHeight, (u) / 16, (v) / 16);
				Insert4(verts, 8 + 16 * dataIndex, x + wStride, y + wantedHeight, (u) / 16 + wuv, (v) / 16);
				Insert4(verts, 12 + 16 * dataIndex, x + wStride, y, (u) / 16 + wuv, (v + 1) / 16);
				x += wStride + spacing;

				if (dataIndex + 1 == TEXT_BATCH) {
					textVBO.setData(16 * TEXT_BATCH, verts);
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

		int charIndex = dataIndex % TEXT_BATCH;
		memset(&verts[16 * charIndex], 0, 16 * (TEXT_BATCH - charIndex) * sizeof(float));

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

		textVBO.setData(16 * TEXT_BATCH, verts);
		textVAO.draw(&textIBO);

		//textBuffer.ModifyVertices(0, 16 * TEXT_BATCH, verts);
		//textBuffer.Draw();
		//rectBuffer.Draw();
	}
	//void DrawRect() {
	//	float vertices[16]{
	//	0,0,0,1,
	//	0,1,0,0,
	//	1,1,1,0,
	//	1,0,1,1,
	//	};
	//	//rectBuffer.ModifyVertices(0, 16, vertices);
	//	//rectBuffer.Draw();
	//}
	//void DrawUVRect(float u, float v, float uw, float vh) {
	//	float vertices[16]{
	//		0,0,u,v,
	//		0,1,u,v+vh,
	//		1,1,u + uw,v + vh,
	//		1,0,u+uw,v
	//	};
	//	//rectBuffer.ModifyVertices(0, 16, vertices);
	//	//rectBuffer.Draw();
	//}

	void DrawCube(glm::mat4 matrix, glm::vec3 scale, glm::vec3 color) {
		cubeObjects.push_back({ glm::scale(matrix,scale),color });
	}
	void DrawNetCube(glm::mat4 matrix, glm::vec3 scale, glm::vec3 color) {
		auto p = [&](float x, float y, float z) {
			return (matrix * glm::translate(glm::vec3(scale.x * (x - .5), scale.y * (y - .5), scale.z * (z - .5))))[3];
		};
		AddLine(p(0, 0, 0), p(1, 0, 0));
		AddLine(p(0, 0, 0), p(0, 0, 1));
		AddLine(p(1, 0, 1), p(1, 0, 0));
		AddLine(p(1, 0, 1), p(0, 0, 1));

		AddLine(p(0, 0, 0), p(0, 1, 0));
		AddLine(p(1, 0, 0), p(1, 1, 0));
		AddLine(p(1, 0, 1), p(1, 1, 1));
		AddLine(p(0, 0, 1), p(0, 1, 1));

		AddLine(p(0, 1, 0), p(1, 1, 0));
		AddLine(p(0, 1, 0), p(0, 1, 1));
		AddLine(p(1, 1, 1), p(1, 1, 0));
		AddLine(p(1, 1, 1), p(0, 1, 1));
	}
	void DrawSphere(glm::vec3 position, float radius, glm::vec3 color) {
	/*	std::vector<glm::vec3> points;
		std::vector<tri> tris;
		std::vector<line> lines;*/

		float pi = glm::pi<float>();
		DrawBegin();
		AddVertex(0, 1*radius, 0);
		AddVertex(0,-1* radius, 0);
		for (int i = 0; i < 5; i++) {
			AddVertex( cosf(2 * pi * (i / 5.f))*radius,.5* radius,sinf(2 * pi * (i / 5.f)) * radius);
			AddVertex( cosf(2 * pi * (1 / 10.f + i / 5.f))* radius,-.5* radius,sinf(2 * pi * (1 / 10.f + i / 5.f)) * radius);

			AddIndex( 2 + i * 2,2 + i * 2 + 1 );
			AddIndex( 2 + ((i + 1) % 5) * 2,2 + i * 2 + 1 );
			AddIndex( 2 + i * 2, 2 + ((i + 1) % 5) * 2 );
			AddIndex( 2 + i * 2 + 1, 2 + ((i + 1) % 5) * 2 + 1 );
			
			AddIndex( 0,2 + i * 2 );
			AddIndex( 1,2 + i * 2 + 1 );
		}
		DrawBuffer();

	}
	struct line {
		int a, b;
	};
	struct tri {
		int a, b, c;
	};
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
	static int writtenVertexPos=0;
	static int writtenIndexPos=0;
	void DrawBegin() {
		writtenVertexPos = 0;
		writtenIndexPos = 0;
	}
	void AddVertex(float x,float y,float z) {
		if (writtenVertexPos + 3 >= RAW_VERTEX_LIMIT|| writtenIndexPos + 2 >= RAW_INDEX_LIMIT)
			return;
		
		rawVertices[writtenVertexPos++] = x;
		rawVertices[writtenVertexPos++] = y;
		rawVertices[writtenVertexPos++] = z;
	}
	void AddIndex(unsigned int a,unsigned int b) {
		if (writtenVertexPos + 3 >= RAW_VERTEX_LIMIT || writtenIndexPos + 2 >= RAW_INDEX_LIMIT)
			return;
		rawIndices[writtenIndexPos++] = a;
		rawIndices[writtenIndexPos++] = b;
	}
	void DrawBuffer() {
		ZeroMemory(rawVertices+writtenVertexPos,sizeof(float)*(RAW_VERTEX_LIMIT-writtenVertexPos-1));
		ZeroMemory(rawIndices+writtenIndexPos,sizeof(uint32_t)*(RAW_INDEX_LIMIT-writtenIndexPos-1));
		// variable in buffer to keep track of which area of the data are zeros
		rawVBO.setData(RAW_VERTEX_LIMIT,rawVertices);
		rawIBO.setData(RAW_INDEX_LIMIT,rawIndices);
		rawVAO.drawLines(&rawIBO);
	}
	static int lines = 0;
	void ClearLines() {
		lines = 0;
	}
	void AddLine(glm::vec3 a, glm::vec3 b) {
		if (lines >= LINE_LIMIT)
			return;

		lineVertices[lines *6]=a.x;
		lineVertices[lines *6+1]=a.y;
		lineVertices[lines *6+2]=a.z;
		lineVertices[lines *6+3]=b.x;
		lineVertices[lines *6+4]=b.y;
		lineVertices[lines *6+5]=b.z;

		lines++;
	}
	void DrawLines() {
		if(lines!=LINE_LIMIT)
			ZeroMemory(lineVertices+lines*6, sizeof(float)*(LINE_LIMIT-lines-1) * 6);

		lineVBO.setData(LINE_LIMIT*6,lineVertices);
		lineVAO.drawLines(&lineIBO);
		lines = 0;
	}

	// global ui pipeline
	namespace ui {
		struct PipeTextBox {
			//TextBox(const std::string& text, float x, float y, float h) : text() {}
			int text_index;
			float x = 0, y = 0, h = 20;
			Font* font = nullptr;
			Color rgba;
			int at = -1;
		};
		// There is a limit to how many rects you can have thanks to float[], having a std::vector and batching rendering would be better
		void Draw(Box box) {
			uiObjects.writeMemory<Box>('B', &box);

			//boxes.push_back(box);
		}
		void Draw(TexturedBox box) {
			if (box.texture == nullptr) {
				return;
			}

			uiObjects.writeMemory<TexturedBox>('T', &box);

			/*if (texturedBoxes.count(box.texture) == 0) {
				texturedBoxes[box.texture] = std::vector<TexturedBox>();
			}
			texturedBoxes[box.texture].push_back(box);*/
		}
		void Draw(TextBox& box) {
			if (!box.edited) {
				box.at = -1;
			} else {
				box.edited = false;
			}

			PipeTextBox a;
			uiStrings.push_back(box.text);
			a.text_index = uiStrings.size()-1;
			a.x = box.x;
			a.y = box.y;
			a.h = box.h;
			a.font = box.font;
			a.rgba = box.rgba;
			a.at = box.at;

			uiObjects.writeMemory<PipeTextBox>('S', &a);

			// texts.push_back(box);
		}

		void Edit(std::string& str, int& at) {
			uint32_t chr;
			if (IsKeyDown(GLFW_KEY_LEFT_CONTROL) && IsKeyPressed(GLFW_KEY_V)) {
				std::string tmp = PollClipboard();
				//str.insert(str.begin() + at, tmp);
				str.insert(at, tmp);
				at += tmp.length();
				return;
			}
			while (chr = PollChar()) {
				if (chr == GLFW_KEY_BACKSPACE) {
					if (str.length() > 0 && at > 0) {
						str = str.substr(0, at - 1) + str.substr(at);
						at--;
					}
				} else if (chr == GLFW_KEY_ENTER) {
					str.insert(str.begin() + at, '\n');
					at++;
				} else if (chr == GLFW_KEY_DELETE) {
					if (str.length() > at) {
						str = str.substr(0, at) + str.substr(at + 1);
					}
				} else if (chr == GLFW_KEY_LEFT) {
					if (at > 0)
						at--;
				} else if (chr == GLFW_KEY_RIGHT) {
					if (at < str.length())
						at++;
				} else {
					str.insert(str.begin() + at, chr);
					at++;
				}
			}
		}
		void Edit(std::string& str) {
			int at = str.length();
			Edit(str, at);
		}
		void Edit(TextBox* box) {
			Edit(box->text);
		}

		static bool inside(Box box, float x, float y) {
			return box.x<x&& box.x + box.w>x && box.y<y&& box.y + box.h>y;
		}
		bool Clicked(Box& box) {
			if (IsKeyPressed(GLFW_MOUSE_BUTTON_1)) {
				float x = GetMouseX();
				float y = GetMouseY();
				if (inside(box, x, y)) {
					return true;
				}
			}
			return false;
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
			if (box.font == nullptr) {
				if (inside({ box.x,box.y,box.h * box.text.length(),box.h }, x, y)) {
					return true;
				}
			} else {

				if (inside({ box.x, box.y, box.font->getWidth(box.text,box.h), box.h }, x, y)) {
					return true;
				}
			}
			return false;
		}
	}
	//void InitUIPipeline() {

	//	AddAsset<Shader>("uiPipeline", new Shader(uiPipelineGLSL));

	//	boxBuffer.setData(4 * VERTEX_SIZE * MAX_RECT_BATCH, nullptr);

	//	uint32_t intArray[6 * MAX_RECT_BATCH];
	//	for (int i = 0; i < MAX_RECT_BATCH; i++) {
	//		intArray[i * 6] = i * 4 + 0;
	//		intArray[i * 6 + 1] = i * 4 + 1;
	//		intArray[i * 6 + 2] = i * 4 + 2;
	//		intArray[i * 6 + 3] = i * 4 + 2;
	//		intArray[i * 6 + 4] = i * 4 + 3;
	//		intArray[i * 6 + 5] = i * 4 + 0;
	//	}
	//	indexBuffer.setData(6 * MAX_RECT_BATCH, intArray);

	//	vertexArray.addAttribute(2); // pos
	//	vertexArray.addAttribute(2); // uv
	//	vertexArray.addAttribute(4); // color
	//	vertexArray.addAttribute(1, &boxBuffer); // texture
	//}
	static struct VERTEX {
		float data[VERTEX_SIZE];
	};
	void RenderUIPipeline() {

		// setup

		Shader* guiShad = GetAsset<Shader>("gui");
		if (!guiShad) return;

		guiShad->bind();
		guiShad->setVec2("uWindow", { GetWidth(), GetHeight() });

		Shader* pipeShad = GetAsset<Shader>("uiPipeline");
		if (!pipeShad) return;

		pipeShad->bind();
		pipeShad->setVec2("uWindow", { GetWidth(), GetHeight() });

		EnableBlend();

		// needs to be done once
		for (int i = 0; i < 8; i++) {
			pipeShad->setInt("uSampler[" + std::to_string(i) + std::string("]"), i);
		}

		// Method one
		int floatIndex = 0;
		std::unordered_map<Texture*, int> boundTextures;
		char lastShader = 'P';
		while (true) {
			char type = uiObjects.readType();
			if (type == 'B') {
				ui::Box* box = uiObjects.readItem<ui::Box>();

				((VERTEX*)&floatArray)[floatIndex * 4 + 0] = { box->x, box->y, 0, 0, box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, -1 };
				((VERTEX*)&floatArray)[floatIndex * 4 + 1] = { box->x, box->y + box->h, 0, 0, box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, -1 };
				((VERTEX*)&floatArray)[floatIndex * 4 + 2] = { box->x + box->w, box->y + box->h, 0, 0, box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, -1 };
				((VERTEX*)&floatArray)[floatIndex * 4 + 3] = { box->x + box->w, box->y, 0, 0, box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, -1 };

				floatIndex++;

			} else if (type == 'T') {
				ui::TexturedBox* box = uiObjects.readItem<ui::TexturedBox>();

				int slot = boundTextures.size();
				if (boundTextures.count(box->texture)) {
					boundTextures[box->texture] = boundTextures.size();
				} else {
					slot = boundTextures[box->texture];
				}

				((VERTEX*)&floatArray)[floatIndex * 4 + 0] = { box->x,			box->y,			box->u,			box->v + box->vh,				box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, (float)slot };
				((VERTEX*)&floatArray)[floatIndex * 4 + 1] = { box->x,			box->y + box->h,	box->u,			box->v,				box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, (float)slot };
				((VERTEX*)&floatArray)[floatIndex * 4 + 2] = { box->x + box->w, box->y + box->h,	box->u + box->uw,	box->v,				box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, (float)slot };
				((VERTEX*)&floatArray)[floatIndex * 4 + 3] = { box->x + box->w, box->y,			box->u + box->uw,	box->v + box->vh,				box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, (float)slot };

				floatIndex++;

			} else if (type == 'S') {
				ui::PipeTextBox* box = uiObjects.readItem<ui::PipeTextBox>();

				if (lastShader == 'P')
					guiShad->bind();
				lastShader = 'G';
				guiShad->setVec2("uPos", { box->x, box->y });
				guiShad->setVec2("uSize", { 1, 1 });
				guiShad->setVec4("uColor", box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a);
				DrawString(box->font, uiStrings[box->text_index], false, box->h, 9999, box->h, box->at);

				// don't continue with other stuff
				continue;
			}

			if (floatIndex == MAX_BOX_BATCH || boundTextures.size() >= 7 || (type == 0 && floatIndex != 0)) {

				for (auto [texture, index] : boundTextures) {
					texture->bind(index);
				}

				if (floatIndex != MAX_BOX_BATCH)
					ZeroMemory(floatArray + floatIndex * 4 * VERTEX_SIZE, (MAX_BOX_BATCH - floatIndex) * 4 * VERTEX_SIZE * sizeof(float));

				if (lastShader == 'G')
					pipeShad->bind();
				lastShader = 'P';

				boxVBO.setData(MAX_BOX_BATCH * 4 * VERTEX_SIZE, floatArray);
				boxVAO.draw(&boxIBO);

				floatIndex = 0;
				boundTextures.clear();
			}
			if (type == 0)
				break;
		}
		uiObjects.clear();
		uiStrings.clear();

#ifdef gone
		// Method two
		if (boxes.size() > 0) {
			int boxIndex = 0;
			for (int i = 0; i < boxes.size(); i++) {
				ui::Box& box = boxes[i];

				((VERTEX*)&floatArray)[boxIndex * 4 + 0] = { box.x, box.y, 0, 0, box.r, box.g, box.b, box.a, -1 };
				((VERTEX*)&floatArray)[boxIndex * 4 + 1] = { box.x, box.y + box.h, 0, 0, box.r, box.g, box.b, box.a, -1 };
				((VERTEX*)&floatArray)[boxIndex * 4 + 2] = { box.x + box.w, box.y + box.h, 0, 0, box.r, box.g, box.b, box.a, -1 };
				((VERTEX*)&floatArray)[boxIndex * 4 + 3] = { box.x + box.w, box.y, 0, 0, box.r, box.g, box.b, box.a, -1 };

				boxIndex++;

				if (boxIndex == MAX_RECT_BATCH || i + 1 == boxes.size()) {
					if (boxIndex != MAX_RECT_BATCH)
						ZeroMemory(floatArray + boxIndex * 4 * VERTEX_SIZE, (MAX_RECT_BATCH - boxIndex) * 4 * VERTEX_SIZE * sizeof(float));

					boxBuffer.setData(MAX_RECT_BATCH * 4 * VERTEX_SIZE, floatArray);
					vertexArray.draw(&indexBuffer);
				}
			}

			// cleanup
			boxes.clear();
		}
		if (texturedBoxes.size() > 0) {

			int textureSlot = 0;
			int boxIndex = 0;
			int totalTextures = 0;
			for (auto [texture, vector] : texturedBoxes) {
				texture->bind(textureSlot);

				for (int i = 0; i < vector.size(); i++) {
					ui::TexturedBox& box = vector[i];

					((VERTEX*)&floatArray)[boxIndex * 4 + 0] = { box.x,			box.y,			box.u,			box.v + box.vh,				box.r, box.g, box.b, box.a, (float)textureSlot };
					((VERTEX*)&floatArray)[boxIndex * 4 + 1] = { box.x,			box.y + box.h,	box.u,			box.v,				box.r, box.g, box.b, box.a, (float)textureSlot };
					((VERTEX*)&floatArray)[boxIndex * 4 + 2] = { box.x + box.w, box.y + box.h,	box.u + box.uw,	box.v,				box.r, box.g, box.b, box.a, (float)textureSlot };
					((VERTEX*)&floatArray)[boxIndex * 4 + 3] = { box.x + box.w, box.y,			box.u + box.uw,	box.v + box.vh,				box.r, box.g, box.b, box.a, (float)textureSlot };

					/*for (int i = 0; i < 4;i++) {
						log::out << floatArray[boxIndex * VERTEX_SIZE * 4 + i * VERTEX_SIZE+2] << " " << floatArray[boxIndex * VERTEX_SIZE * 4 + i * VERTEX_SIZE+3] << "\n";
					}*/

					boxIndex++;
					if (boxIndex == MAX_RECT_BATCH) {
						boxIndex = 0;

						boxBuffer.setData(MAX_RECT_BATCH * 4 * VERTEX_SIZE, floatArray);
						vertexArray.draw(&indexBuffer);
					}
				}
				textureSlot++;
				totalTextures++;
				if ((textureSlot == 8 || totalTextures == texturedBoxes.size()) && boxIndex != 0) {
					textureSlot = 0;
					ZeroMemory(floatArray + boxIndex * 4 * VERTEX_SIZE, (MAX_RECT_BATCH - boxIndex) * 4 * VERTEX_SIZE * sizeof(float));

					boxBuffer.setData(MAX_RECT_BATCH * 4 * VERTEX_SIZE, floatArray);
					vertexArray.draw(&indexBuffer);
				}
			}

			// cleanup
			texturedBoxes.clear();
		}
		if (texts.size() > 0) {
			Shader* shad = GetAsset<Shader>("gui");
			if (!shad) return;

			EnableBlend();
			shad->bind();
			shad->setVec2("uWindow", { GetWidth(), GetHeight() });

			Font* font = GetAsset<Font>("consolas");
			for (int i = 0; i < texts.size(); i++) {
				ui::TextBox& text = texts[i];

				shad->setVec2("uPos", { text.x, text.y });
				shad->setVec2("uSize", { 1, 1 });
				shad->setVec4("uColor", text.r, text.g, text.b, text.a);
				DrawString(font, text.text, false, text.h, 9999, text.h, text.at);
			}

			texts.clear();
		}
#endif
	}
}