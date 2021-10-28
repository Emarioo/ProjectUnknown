#include "gonpch.h"

//#if ENGONE_GLFW

#include "GUIManager.h"

namespace engone
{
	IProperty::IProperty(IElement* element)
		: element(element){}

	IColor::IColor(IElement* element) : IProperty(element) {}
	void IColor::Set(float gray, float alpha)
	{
		r = gray;
		g = gray;
		b = gray;
		a = alpha;
	}
	void IColor::Set(int gray, int alpha)
	{
		r = gray / 255.f;
		g = gray / 255.f;
		b = gray / 255.f;
		a = alpha / 255.f;
	}
	void IColor::Set(float red, float green, float blue, float alpha)
	{
		r = red;
		g = green;
		b = blue;
		a = alpha;
	}
	void IColor::Set(int red, int green, int blue, int alpha)
	{
		r = red / 255.f;
		g = green / 255.f;
		b = blue / 255.f;
		a = alpha / 255.f;
	}
	ITexture::ITexture(IElement* element) : IProperty(element){}
	void ITexture::Set(Texture* texture)
	{
		this->texture = texture;
	}
	IX::IX(IElement* element) : IProperty(element){}
	IX* IX::Center(float value, bool fixed, IElement* stick)
	{
		side = 0;
		this->value = value;
		this->fixed = fixed;
		this->attached = stick;
		return this;
	}
	IX* IX::Left(float value, bool fixed, IElement* stick)
	{
		side = -1;
		this->value = value;
		this->fixed = fixed;
		this->attached = stick;
		return this;
	}
	IX* IX::Right(float value, bool fixed, IElement* stick)
	{
		side = 1;
		this->value = value;
		this->fixed = fixed;
		this->attached = stick;
		return this;
	}
	float IX::Value()
	{
		float offset = value;
		if (!fixed) {
			offset = value / 1920 * Width(); // Non-fixed value
		}
		if (side == 1) { // right
			if (attached == nullptr) return Width() - offset - element->finalW;
			else return attached->finalX + offset - element->finalW;
		}
		else if (side == -1) {// left
			if (attached == nullptr) return offset;
			else return attached->finalX + attached->finalW + offset;
		}
		else {// center
			if (attached == nullptr) return (Width()-element->finalW)/2+ offset;
			else return attached->finalX+(attached->finalW - element->finalW) / 2 + offset;
		}
	}
	IY::IY(IElement* element) : IProperty(element) {}
	IY* IY::Center(float value, bool fixed, IElement* stick)
	{
		side = 0;
		this->value = value;
		this->fixed = fixed;
		this->attached = stick;
		return this;
	}
	IY* IY::Bottom(float value, bool fixed, IElement* stick)
	{
		side = -1;
		this->value = value;
		this->fixed = fixed;
		this->attached = stick;
		return this;
	}
	IY* IY::Top(float value, bool fixed, IElement* stick)
	{
		side = 1;
		this->value = value;
		this->fixed = fixed;
		this->attached = stick;
		return this;
	}
	float IY::Value()
	{
		float offset = value;
		if (!fixed) {
			offset = value / 1080 * Height(); // Non-fixed value
		}
		if (side == 1) { // right
			if (attached == nullptr) return Height() - offset - element->finalH;
			else return attached->finalY + offset - element->finalH;
		}
		else if (side == -1) {// left
			if (attached == nullptr) return offset;
			else return attached->finalY + attached->finalH + offset;
		}
		else {// center
			if (attached == nullptr) return (Height() - element->finalH) / 2 + offset;
			else return attached->finalY + (attached->finalH - element->finalH) / 2 + offset;
		}
	}
	IW::IW(IElement* element) : IProperty(element) {}
	IW* IW::Center(float value, bool fixed, IElement* stick)
	{
		this->value = value;
		this->fixed = fixed;
		this->attached = stick;
		return this;
	}
	float IW::Value()
	{
		float offset = value;
		if (!fixed) {
			offset = value / 1920 * Width(); // Non-fixed value
		}
		
		if (attached == nullptr) return offset;
		else return attached->finalW + offset;
	}
	IH::IH(IElement* element) : IProperty(element) {}
	IH* IH::Center(float value, bool fixed, IElement* stick)
	{
		this->value = value;
		this->fixed = fixed;
		this->attached = stick;
		return this;
	}
	float IH::Value()
	{
		float offset = value;
		if (!fixed) {
			offset = value / 1080 * Height(); // Non-fixed value
		}
		if (attached == nullptr) return offset;
		else return attached->finalH + offset;
	}
	IElement::IElement(const std::string& name) : name(name){}
	template <class T>
	T* IElement::Property()
	{
		IProperty* prop = properties[T::type];
		if (prop != nullptr) {
			return static_cast<T*>(prop);
		}
		else {
			T* t = new T(this);
			properties[T::type] = t;
			return t;
		}
	}
	bool IElement::_OnEvent(Event& e)
	{
		if (OnEvent(e)) {
			return true;
		}
		for (int i = 0; i < children.size(); i++) {
			children[i]->_OnEvent(e);
		}
	}
	void IElement::_OnUpdate(float delta)
	{
		finalW = Property<IW>()->Value();
		finalH = Property<IH>()->Value();
		finalX = Property<IX>()->Value();
		finalY = Property<IY>()->Value();
		OnUpdate(delta);
		for (int i = 0; i < children.size(); i++) {
			children[i]->_OnUpdate(delta);
		}
	}
	void IElement::_OnRender()
	{
		OnRender();
		for (int i = 0; i < children.size();i++) {
			children[i]->_OnRender();
		}
	}
	IPanel::IPanel(const std::string& name) : IElement(name){}
	void IPanel::OnRender()
	{
		IColor* color = Property<IColor>();

		if (color->a != 0) { // panel is transparent
			Shader* gui = GetAsset<Shader>("gui");
			gui->SetVec2("uPos", { finalX, finalY });
			gui->SetVec2("uSize", { finalW, finalH });
			gui->SetVec4("uColor", color->r, color->g, color->b, color->a);

			Texture* texture = Property<ITexture>()->texture;
			if (texture != nullptr) {
				texture->Bind();
				gui->SetInt("uTextured", 1);
			}
			else
				gui->SetInt("uTextured", 0);

			DrawRect();
		}
	}
	static std::vector<IElement*> elements;
	static Shader* guiShader;
	static const std::string guiShaderSource = {
#include "../Shaders/gui.glsl"
	};
	void InitGUI()
	{
		guiShader = new Shader(guiShaderSource, true);
		AddAsset<Shader>("gui", guiShader);
		AddListener(new Listener(EventType::Click | EventType::Move | EventType::Scroll | EventType::Key, [](Event& e) {
			for (int i = 0; i < elements.size();i++) {
				if (elements[i]->_OnEvent(e)) {
					return true;
				}
			}
			return false;
			}));
	}
	void AddElement(IElement* element)
	{
		int insertIndex = -1;
		for (int i = 0; i < elements.size(); i++) {
			if (elements[i] == element)
				return;
			if (insertIndex == -1 && element->priority > elements[i]->priority)
				insertIndex = i;
		}
		if (insertIndex == -1)
			elements.push_back(element);
		else
			elements.insert(elements.begin() + insertIndex, element);
	}
	IElement* GetElement(const std::string& name)
	{
		for (int i = 0; i < elements.size(); i++) {
			if (elements[i]->name == name) {
				return elements[i];
			}
		}
		return nullptr;
	}
	void UpdateUI(float delta)
	{
		for (int i = 0; i < elements.size(); i++) {
			elements[i]->_OnUpdate(delta);
		}
	}
	void RenderUI()
	{
		guiShader->Bind();
		guiShader->SetVec2("uWindow", {Width(),Height()});
		for (int i = 0; i < elements.size();i++) {
			elements[i]->_OnRender();
		}
	}


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

	//-- namespace global stuff

	/*static const std::string guiShaderSource = {
#include "../Shaders/gui.glsl"
	};
	*/
	//static Shader* guiShader;
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
			if (panels[i]->name == name) {
				return panels[i];
			}
		}
		return nullptr;
	}/*
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
	}*/
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