#pragma once

#include "Text.h"
#include <functional>
#include <vector>

#include "ITransition.h"
#include "IBase.h"

namespace engine {
	class IElement : public IBase {
	public:
		IElement(const std::string& name, int priority);
		~IElement();

		std::string texture = "blank";
		void Texture(const std::string& texture);

		int priority = 0;

		// What is this?
		bool isBlank = false;
		void Blank();

		Text text;
		void SetText(const std::string& s);
		const std::string& GetText();
		/*
		Fit text in this element - More explanations
		*/
		void Text(Font* f, const std::string& s, const IColor& color);
		void Text(Font* f, const std::string& s, int charHeight, const IColor& color);
		void Text(Font* f, const std::string& s, float charHeight, const IColor& color);

		bool isEditable = false;
		void SetEditable();

		// Explanation for these?
		std::vector<bool*> tags;
		std::vector<std::function<bool()>> tagsF;
		/*
		Optimize this function?
		It runs on every update call...
		*/
		bool HasTags();
		void AddTag(bool*);
		void AddTag(std::function<bool()>);

		std::vector<ITransition> transitions;
		ITransition* NewTransition(bool* activator);
		ITransition* NewTransition(std::function<bool()> activator);

		/*
		True if mouse is held on element.
		*/
		bool isHolding = false;
		/*
		True if item has been clicked on. Used for typing text
		*/
		bool isSelected = false;
		/*
		True if Mouse is hovering over element.
		*/
		bool isHovering = false;

		std::function<void(int mx, int my, int button, int action)> OnMouse = nullptr;
		bool MouseEvent(int mx, int my, int button, int action) override;

		std::function<void(double scroll)> OnScroll = nullptr;
		bool ScrollEvent(double scroll) override;

		std::function<void(int mx, int my)> OnHover = nullptr;
		bool HoverEvent(int mx, int my) override;

		std::function<void(int key, int action)> OnKey = nullptr;
		bool KeyEvent(int key, int action) override;

		/*
		Used for updating transitions. (Seperation from the virtual update function)
		*/
		void InternalUpdate(float delta);
		/*
		Can be changed if IElement is a derived class
		*/
		virtual void Update(float delta);
		/*
		Can be changed if IElement is a derived class
		*/
		virtual void Render();

		bool Inside(float mx, float my);
	};
}