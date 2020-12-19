#pragma once

#include "Managers/DataManager.h"
#include "Managers/ObjectManager.h"
#include "UI/IElement.h"

#include <unordered_map>

/*
Global Manager for 2D elements (buttons, text)
 The iManager stores these elements in a global acceccible vector
 Two types of rendering (start menu buttons, in game inventories)
 Also calls functions like "LoadWorld" from ObjectManager
*/
namespace iManager {

	bool IsPauseMode();
	void SetPauseMode(bool f);

	void UIFadeOn();
	void UIFadeOff();

	/*
	Setup callbacks and 2D elements
	*/

	void Init();
	void UpdateInterface(float delta);
	void RenderInterface();

	IElement* NewElement(const std::string& name,int priority);
	/*
	Find element with name
	(Avoid calling to many times)
	*/
	IElement* GetElement(const std::string& s);

	Font* GetFont();
}