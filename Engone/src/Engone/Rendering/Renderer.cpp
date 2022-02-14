#include "gonpch.h"

#include "Renderer.h"
#include "../Handlers//AssetHandler.h"

namespace engone {

	static Shader* guiShader=nullptr;
	static const int TEXT_BATCH = 40;
	static float verts[4 * 4 * TEXT_BATCH];
	static TriangleBuffer textBuffer, rectBuffer;

	int colliderVertexLimit = 500, colliderIndexLimit = 3000;
	LineBuffer colliderBuffer;
	float* colliderVertices;
	unsigned int* colliderIndices;

	void InitRenderer() {
		guiShader = GetAsset<Shader>("gui");

		std::uint32_t indes[TEXT_BATCH * 6];
		for (int i = 0; i < TEXT_BATCH; i++) {
			indes[0 + 6 * i] = 0 + 4 * i;
			indes[1 + 6 * i] = 1 + 4 * i;
			indes[2 + 6 * i] = 2 + 4 * i;
			indes[3 + 6 * i] = 2 + 4 * i;
			indes[4 + 6 * i] = 3 + 4 * i;
			indes[5 + 6 * i] = 0 + 4 * i;
		}
		textBuffer.Init(true, nullptr, 4 * 4 * TEXT_BATCH, indes, 6 * TEXT_BATCH);
		textBuffer.SetAttrib(0, 4, 4, 0);
		float temp[]{ // This will be updated in when using DrawRect or DrawUVRect
			0, 0, 0, 0,
			0, 1, 0, 1,
			1, 1, 1, 1,
			1, 0, 1, 0,
		};
		unsigned int temp2[]{
			0,1,2,
			2,3,0
		};
		rectBuffer.Init(true, temp, 16, temp2, 6);
		rectBuffer.SetAttrib(0, 4, 4, 0);

		colliderVertices = new float[colliderVertexLimit * 3];// {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1}; not using color
		colliderIndices = new unsigned int[colliderIndexLimit * 2];// {0, 1, 0, 2, 0, 3};
		
		memset(colliderVertices,0,colliderVertexLimit*3);
		memset(colliderIndices,0,colliderIndexLimit*2);
		
		colliderBuffer.Init(true, colliderVertices, 3 * colliderVertexLimit, colliderIndices, 2 * colliderIndexLimit);
		colliderBuffer.SetAttrib(0, 3, 3, 0);
	}
	void UninitRenderer() {
		delete[] colliderVertices;
		delete[] colliderIndices;
		colliderBuffer.Uninit();
	}
	void EnableBlend() {
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}
	void EnableDepth() {
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
	static void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3)
	{
		ar[ind] = f0;
		ar[ind + 1] = f1;
		ar[ind + 2] = f2;
		ar[ind + 3] = f3;
	}
	void DrawString(Font* font, const std::string& text, bool center, float wantedHeight, float maxWidth, float maxHeight, int atChar)
	{
		if (text.length() == 0 && atChar == -1)
			return;
		if (font == nullptr) {
			std::cout << "DrawString: Font is null\n";
			return;
		}
		else if (font->texture.error) {
			return;
		}
		else {
			if(guiShader!=nullptr)
				guiShader->SetInt("uColorMode", 1);
			font->texture.Bind();
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

				Insert4(verts, 16 * dataIndex, x, y, (u) / 16, (v+1) / 16);
				Insert4(verts, 4 + 16 * dataIndex, x, y+ wantedHeight, (u) / 16, (v) / 16);
				Insert4(verts, 8 + 16 * dataIndex, x + wStride, y+ wantedHeight, (u) / 16 + wuv, (v) / 16);
				Insert4(verts, 12 + 16 * dataIndex, x + wStride, y, (u) / 16 + wuv, (v+1) / 16);
				x += wStride + spacing;

				if (dataIndex == TEXT_BATCH) {
					textBuffer.ModifyVertices(0, 4 * 4 * TEXT_BATCH, verts);
					textBuffer.Draw();
					dataIndex = 0;
				}
				else {
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

		textBuffer.ModifyVertices(0, 16 * TEXT_BATCH, verts);
		textBuffer.Draw();
		//rectBuffer.Draw();
	}
	void DrawRect() {
		float vertices[16]{
		0,0,0,1,
		0,1,0,0,
		1,1,1,0,
		1,0,1,1,
		};
		rectBuffer.ModifyVertices(0, 16, vertices);
		rectBuffer.Draw();
	}
	void DrawUVRect(float u, float v, float uw, float vh) {
		float vertices[16]{
			0,0,u,v,
			0,1,u,v+vh,
			1,1,u + uw,v + vh,
			1,0,u+uw,v
		};
		rectBuffer.ModifyVertices(0, 16, vertices);
		rectBuffer.Draw();
	}

	void DrawCube(float w,float h,float d) {
		w /= 2;
		h /= 2;
		d /= 2;

		float v[]{
			-w,-h,-d,
			w,-h,-d,
			w,-h,d,
			-w,-h,d,

			-w,h,-d,
			w,h,-d,
			w,h,d,
			-w,h,d,
		};
		unsigned int ind[]{
			0,1, 1,2, 2,3, 3,0,
			0,4, 1,5, 2,6, 3,7,
			4,5, 5,6, 6,7, 7,4,
		};
		colliderBuffer.ModifyVertices(0, sizeof(v)/sizeof(float), v);
		colliderBuffer.ModifyIndices(0, sizeof(ind)/sizeof(unsigned int), ind);
		colliderBuffer.Draw();
	}
	struct line {
		int a, b;
	};
	struct tri {
		int a, b, c;
	};
	void DrawSphere(float radius) {
		std::vector<glm::vec3> points;
		std::vector<tri> tris;
		std::vector<line> lines;
		
		float pi = glm::pi<float>();
		points.push_back({0,1,0});
		points.push_back({0,-1,0});
		for (int i = 0; i < 5;i++) {
			points.push_back({ cosf(2*pi*(i/5.f)),.5,sinf(2*pi*(i/5.f)) });
			points.push_back({ cosf(2 * pi * (1/10.f+ i / 5.f)),-.5,sinf(2 * pi * (1/10.f+i / 5.f)) });
			
			lines.push_back({ 2 + i * 2,2 + i * 2 + 1 });
			lines.push_back({ 2 + ((i+1)%5) * 2,2 + i * 2 + 1 });
			
			lines.push_back({ 2+i*2, 2+((i+1)%5) * 2 });
			lines.push_back({ 2+i*2+1, 2+((i+1)%5) * 2+1 });

			lines.push_back({0,2+i*2});
			lines.push_back({ 1,2+i*2+1 });
		}

		for (glm::vec3& p : points)
			p *= radius;

		{
			//std::vector<glm::vec3> oldPoints=points;
			//std::vector<line> oldLines=lines;
			//std::vector<tri> oldTris=tris;

			
		}

		colliderBuffer.ModifyVertices(0, points.size()*3, points.data());
		colliderBuffer.ModifyIndices(0, lines.size()*2, lines.data());
		colliderBuffer.Draw();
	}
}