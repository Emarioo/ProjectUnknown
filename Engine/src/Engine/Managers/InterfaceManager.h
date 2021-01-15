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
namespace engine {

	bool GetPauseMode();
	bool* GetPauseModeRef();
	void SetPauseMode(bool f);

	void UIFadeOn();
	void UIFadeOff();

	/*
	Setup event callbacks
	 Init after renderer
	*/
	void InitInterface();
	void UpdateInterface(float delta);
	void RenderInterface();

	/*
	Elements take priority over these callbacks
	 give nullptr as argument if you won't be using event
	*/
	void GetEventCallbacks(
		std::function<void(int, int)> key,
		std::function<void(double, double, int, int,const std::string&)> mouse,
		std::function<void(double)> scroll,
		std::function<void(double, double)> drag);

	/*
	AddFont function?
	*/

	IElement* NewElement(const std::string& name,int priority);
	/*
	Find element with name
	(Avoid calling to many times)
	*/
	IElement* GetElement(const std::string& s);

	Font* GetFont();
}