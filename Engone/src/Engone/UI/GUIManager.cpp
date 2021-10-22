#include "gonpch.h"

//#if ENGONE_GLFW

#include "GUIManager.h"

namespace engone
{
	float ConstraintH::Value()
	{
		float move = raw;

		if (parent == nullptr) return move;
		else return parent->renderH + move;
	}
	float ConstraintW::Value()
	{
		float move = raw;

		if (parent == nullptr) return move;
		else return panel->renderW + move;
	}
	float ConstraintY::Value()
	{
		float move = raw;

		if (side == 0) {
			if (parent == nullptr) return move - panel->renderH / 2 + Height() / 2;
			else return parent->renderY + move;
		}
		else if (side == 1) {
			if (parent == nullptr) return move;// - panel->renderH;
			else return parent->renderY + move + parent->renderH;
		}
		else {
			if (parent == nullptr) return Height() - move - panel->renderH;
			else return parent->renderY - panel->renderH - move;
		}
	}
	float ConstraintX::Value()
	{
		float move = raw;

		if (side == 0) {
			if (parent == nullptr) return move - panel->renderW / 2 + Width() / 2;
			else return parent->renderX + move;
		}
		else if (side == 1) {
			if (parent == nullptr) return Width() - move - panel->renderW;
			else return parent->renderX - move - panel->renderW;
		}
		else {
			if (parent == nullptr) return move;
			else return parent->renderX + parent->renderW + move;
		}
	}

	IElement::IElement(const std::string& name) : name(name)
	{
	}
	template <class T>
	T* IElement::Component()
	{
		IComponent* out = components[T::type];
		if (out != nullptr) {
			return static_cast<T>(out);
		}
		else {
			T* t = new T();
			components[T::type] = t;
			return t;
		}
	}

	//-- namespace global stuff

	static const std::string guiShaderSource = {
#include "../Shaders/gui.glsl"
	};

	static Shader* guiShader;
	static std::vector<Panel*> panels;
	void AddPanel(Panel* panel)
	{
		int insertIndex = -1;
		for (int i = 0; i < panels.size(); i++) {
			if (panels[i] == panel)
				return;
			if (insertIndex == -1 && panel->priority > panels[i]->priority)
				insertIndex = i;
		}
		if (insertIndex == -1)
			panels.push_back(panel);
		else
			panels.insert(panels.begin() + insertIndex, panel);
	}
	Panel* GetPanel(const std::string& name)
	{
		for (int i = 0; i < panels.size();i++) {
			if (panels[i].name == name) {
				return panels[i];
			}
		}
		return nullptr;
	}
	void InitGUI()
	{
		guiShader = new Shader(guiShaderSource, true);
		AddShader("gui", guiShader);
		
		AddListener(new Listener(EventType::Click | EventType::Move | EventType::Key | EventType::Scroll, [](Event& e) {
			for (int i = 0; i < panels.size(); i++) {
				if (panels[i]->OnEvent(e))
					return true;
			}
			return false;
			}));
	}
	void RenderPanels()
	{
		guiShader->Bind();
		guiShader->SetVec2("uWindow", { Width(),Height() });

		for (int i = 0; i < panels.size(); i++) {
			panels[i]->OnRender();
		}
	}
	void UpdatePanels(float delta)
	{
		for (int i = 0; i < panels.size(); i++) {
			panels[i]->OnUpdate(delta);
		}
	}
}
//#endif