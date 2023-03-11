#include "Engone/Rendering/UIRenderer.h"

#include "Engone/Window.h"
#include "Engone/Rendering/CommonRenderer.h"

// #define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Engone/Shaders/ShaderGLSL.h"

// static const char* uiPipelineGLSL = {
// #include "Engone/Shaders/uiPipeline.glsl"
// };
// static const char* rendererGLSL = {
// #include "Engone/Shaders/renderer.glsl"
// };
// static const char* linesGLSL = {
// #include "Engone/Shaders/lines3d.glsl"
// };
// static const char* guiShaderSource = {
// #include "Engone/Shaders/gui.glsl"
// };
// static const char* objectSource = {
// #include "Engone/Shaders/object.glsl"
// };
// static const char* armatureSource = {
// #include "Engone/Shaders/armature.glsl"
// };
// static const char* particleGLSL = {
// #include "Engone/Shaders/particle.glsl"
// };

/*
Example uses
	
newlayout("",depth)
	
id0 = drawbox("",x,y,w,h,color)	

id1 = drawbox("",id0,alignbelow,offset)

id2 = drawbox("",)
id3 = drawtext("",)

layout("",)

*/

namespace engone {

	struct PIPE_VERTEX {
		float data[UIRenderer::VERTEX_SIZE];
	};
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
			uint32_t text_index = 0;
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
			if (!GetActiveUIRenderer()) return;
				Window* win = GetActiveUIRenderer()->m_owner;
			if (box.x + box.w < 0 || box.x>win->getWidth() || box.y + box.h<0 || box.y>win->getHeight())
				return; // out of bounds
			UIRenderer ead;
			GetActiveUIRenderer()->uiObjects.writeMemory<Box>('B', &box);
		}
		void Draw(TexturedBox box) {
			if (box.texture == nullptr) {
				return;
			}
			if (!GetActiveUIRenderer()) return;
				Window* win = GetActiveUIRenderer()->m_owner;
			if (box.x + box.w < 0 || box.x>win->getWidth() || box.y + box.h<0 || box.y>win->getHeight())
				return; // out of bounds
			GetActiveUIRenderer()->uiObjects.writeMemory<TexturedBox>('T', &box);
		}
		void Draw(TextBox& box) {
			if (!box.editing) {
				box.at = -1;
			}
			if (!GetActiveUIRenderer()) return;
			Window* win = GetActiveUIRenderer()->m_owner;
			// ISSUE: width of str is used.  ( box.x+box.w<0 )
			float h = box.getHeight();
			if (box.x > win->getWidth() || box.y + h<0 || box.y>win->getHeight())
				return; // out of bounds
			PipeTextBox a;
			GetActiveUIRenderer()->uiStrings.push_back(box.text);
			a.text_index = GetActiveUIRenderer()->uiStrings.size() - 1;
			a.x = box.x;
			a.y = box.y;
			a.h = box.h;
			a.font = box.font;
			a.rgba = box.rgba;
			a.at = box.at;

			GetActiveUIRenderer()->uiObjects.writeMemory<PipeTextBox>('S', &a);
		}
		void Draw(ModelAsset* asset, glm::mat4 matrix) {
			if (!GetActiveUIRenderer()) return;
			ModelBox box = { 0,matrix };
			GetActiveUIRenderer()->uiStrings.push_back(asset->getLoadName());
			box.model_text_index = GetActiveUIRenderer()->uiStrings.size() - 1;
			GetActiveUIRenderer()->uiObjects.writeMemory<ModelBox>('M', &box);
		}

		void Edit(std::string& str, int& at, bool& editing, bool stopEditWithEnter) {
			if (at < 0) at = str.length();
			if (at > str.length()) at = str.length();
			uint32_t chr = 0;
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
			bool editing = false;
			Edit(str, at, editing, true);
		}
		// will only edit if box->editing is true.
		// the TextBox need to store it's editing value for next frame/update.
		// either make your own isEditing variable and then box->editing=isEditing.
		// or have TextBox in a class or as a static global variable. It's up to you.
		void Edit(TextBox* box, bool stopEditWithEnter) {
			if (box->editing) {
				Edit(box->text, box->at, box->editing, stopEditWithEnter);
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
					box.at = ((x - box.x) / wid + 0.5f);
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
	VertexBuffer& UIRenderer::getInstanceBuffer() { return instanceBuffer; }

	void UIRenderer::drawLine(float x, float y, float x1, float y1, ui::Color color){
		lines.push_back({x,y,x1,y1,color});
	}
	void UIRenderer::render(LoopInfo& info) {
		EnableBlend();
		
		float sw = info.window->getWidth();
		float sh = info.window->getHeight();
		
		if (!instanceBuffer.initialized()) {
			instanceBuffer.setData(INSTANCE_BATCH * sizeof(glm::mat4), nullptr);
		}
		if (!boxVBO.initialized()) {
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
			boxIBO.setData(6 * MAX_BOX_BATCH * sizeof(unsigned int), intArray);

			boxVAO.addAttribute(2); // pos
			boxVAO.addAttribute(2); // uv
			boxVAO.addAttribute(4); // color
			boxVAO.addAttribute(1, &boxVBO); // texture
		}

		if(!lineVBO.initialized()){
			lineVBO.setData(2*VERTEX_SIZE*MAX_LINE_BATCH*sizeof(float),nullptr);	
			
			lineVAO.addAttribute(2); // pos
			lineVAO.addAttribute(2); // uv
			lineVAO.addAttribute(4); // color
			lineVAO.addAttribute(1, &lineVBO); // texture
		}

		// Window* win = engone::GetActiveWindow();
		CommonRenderer* renderer = info.window->getCommonRenderer();
		Camera* camera = renderer->getCamera();
		
		// setup
		ShaderAsset* guiShad = info.window->getStorage()->get<ShaderAsset>("gui");
		if (!guiShad) {
			log::out << "UIRenderer : Missing gui shader\n";
			return;
		}

		guiShad->bind();
		guiShad->setVec2("uWindow", { sw, sh });

		ShaderAsset* pipeShad = info.window->getStorage()->get<ShaderAsset>("uiPipeline");
		if (!pipeShad) {
			log::out << "UIRenderer : Missing uiPipeline shader\n";
			return;
		}

		pipeShad->bind();
		pipeShad->setVec2("uWindow", { sw, sh });
		// needs to be done once
		for (int i = 0; i < 8; i++) {
			pipeShad->setInt("uSampler[" + std::to_string(i) + std::string("]"), i);
		}

		ShaderAsset* objectShad = info.window->getStorage()->get<ShaderAsset>("object");
		if (!objectShad) {
			log::out << "UIRenderer : Missing object shader\n";
			return;
		}

		objectShad->bind();
		renderer->updateOrthogonal(objectShad, glm::mat4(1));
		objectShad->setVec3("uCamera", camera->getPosition());

		ShaderAsset* armatureShad = info.window->getStorage()->get<ShaderAsset>("armature");
		if (!armatureShad) {
			log::out << "UIRenderer : Missing armature shader\n";
			return;
		}

		armatureShad->bind();
		renderer->updateOrthogonal(armatureShad, glm::mat4(1));
		armatureShad->setVec3("uCamera", camera->getPosition());

		// Method one
		int floatIndex = 0;
		std::unordered_map<Texture*, int> boundTextures; // Todo: Use an array instead of a map. There will only be 8 elements in the array.
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

				// New good code
				int slot = 0;
				auto find = boundTextures.find(box->texture);
				if(find==boundTextures.end()){
					slot = boundTextures[box->texture] = boundTextures.size();
				}else{
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

			if (floatIndex == MAX_BOX_BATCH || boundTextures.size() >= 7 || (type == 0 && floatIndex != 0) || ((type == 'S' || type == 'M') && lastShader != 'P' && lastShader != 0)) {

				for (auto& pair : boundTextures) {
					pair.first->bind(pair.second);
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
				guiShad->setVec4("uColor", box->rgba.toVec4());

				//DrawString(box->font, uiStrings[box->text_index], false, box->h, 9999, 9999, box->at);
				renderer->drawString(box->font, uiStrings[box->text_index], box->h, box->at);

				// don't continue with other stuff
				continue;
			}
			if (type == 'M') {
				ui::ModelBox* box = uiObjects.readItem<ui::ModelBox>();

				lastShader = 'O';

				ModelAsset* modelAsset = info.window->getStorage()->get<ModelAsset>(uiStrings[box->model_text_index]);
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
				ShaderAsset* shader = info.window->getStorage()->get<ShaderAsset>("object");
				if (!shader->getError()) {
					shader->bind();

					//bindLights(shader, { 0,0,0 });
					shader->setMat4("uTransform", glm::mat4(0)); // zero in mat4 means we do instanced rendering. uTransform should be ignored
					for (auto& pair : normalObjects) {
						auto& asset = pair.first;
						auto& vector = pair.second;
						for (uint32_t j = 0; j < asset->materials.size(); ++j) {// Maximum of 4 materials
							asset->materials[j]->bind(shader, j);
						}
						uint32_t remaining = vector.size();
						uint32 batchSize = UIRenderer::INSTANCE_BATCH;
						while (remaining > 0) {
							uint32_t batchAmount = std::min(remaining, batchSize);

							getInstanceBuffer().setData(batchAmount * sizeof(glm::mat4), (vector.data() + vector.size() - remaining));

							asset->vertexArray.selectBuffer(3, &getInstanceBuffer());
							asset->vertexArray.draw(&asset->indexBuffer, batchAmount);
							remaining -= batchAmount;
						}
					}
					normalObjects.clear();
				}
				shader = info.window->getStorage()->get<ShaderAsset>("armature");
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
		
		pipeShad->bind();
		//renderer->updateOrthogonal(pipeShad, glm::mat4(1));
		EnableBlend();

		glLineWidth(2.f);

		int lineIndex=0;
		while(true){
			// MAX_LINE_BATCH
			
			// lineVBO.setData(MAX_LINE_BATCH*));
			if(lineIndex==lines.size())
				break;
				
			Line& line = lines[lineIndex];
			lineIndex++;
			((PIPE_VERTEX*)&floatArray)[floatIndex * 2 + 0] = { line.x, line.y, 0, 0, line.color.r, line.color.g, line.color.b, line.color.a, -1 };
			((PIPE_VERTEX*)&floatArray)[floatIndex * 2 + 1] = { line.x1, line.y1, 0, 0, line.color.r, line.color.g, line.color.b, line.color.a, -1 };
			floatIndex++;		

			if(floatIndex == MAX_LINE_BATCH || lineIndex==lines.size()){

				if (floatIndex != MAX_LINE_BATCH)
					// Note: floatArray's size is based on MAX_BOX_BATCH. That's why we mix box and line. Line rendering is borrowing floatArray.
					ZeroMemory(floatArray + floatIndex * 2 * VERTEX_SIZE, (MAX_BOX_BATCH*4 - floatIndex*2) * VERTEX_SIZE * sizeof(float));

				// time to render
				lineVBO.setData(MAX_LINE_BATCH*VERTEX_SIZE*2*sizeof(float),floatArray);
				
				lineVAO.drawLineArray(floatIndex*2);
				floatIndex=0;
			}
		}
		lines.clear();
	}
	
	static UIRenderer* s_activeUIRenderer = nullptr;
	void UIRenderer::setActiveUIRenderer() {
		s_activeUIRenderer = this;
	}
	UIRenderer* GetActiveUIRenderer() {
		return s_activeUIRenderer;
	}
	UIRenderer::~UIRenderer() {
		if (s_activeUIRenderer == this)
			s_activeUIRenderer = nullptr;
	}
}