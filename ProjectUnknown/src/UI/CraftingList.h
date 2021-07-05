#pragma once

#include "Engine/UI/IBase.h"
#include "Items/Container.h"

class CraftingList : public engine::IBase {
public:
	CraftingList(const std::string& name);

	bool active;
	bool IsActive();

	bool ClickEvent(int mx, int my, int button, int action) override;
	bool ScrollEvent(double scroll) override;
	bool KeyEvent(int key, int action) override;

	void Update(float delta) override;
	void Render() override;

	/*
	Holds possible crafting attempts. Zero if no attempts.
	*/
	int craftCount=1;

	float scrollingSensitivity=0.025f;
	float scrolling;
	int selectedCategory=0;
	int selectedRecipe=-1;

	//Container* container = nullptr; // Reference to current container to be shown.
};