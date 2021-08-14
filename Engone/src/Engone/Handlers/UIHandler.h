#pragma once

#include "AssetHandler.h"
#include "../UI/IElement.h"

/*
Global Manager for 2D elements (buttons, text)
 The iManager stores these elements in a global acceccible vector
 Two types of rendering (start menu buttons, in game inventories)
 Also calls functions like "LoadWorld" from ObjectManager
*/
namespace engine {

	/*
	Setup event callbacks and fonts?
	 Init after renderer
	*/
	void InitInterface();
	void UpdateInterface(double delta);
	void RenderInterface(double lag);

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

	void AddBase(IBase* s);
	IBase* GetBase(const std::string& name);

	IElement* AddElement(IElement* element);
	IElement* AddElement(const std::string& name,int priority);
	/*
	Find element with name
	(Avoid calling to many times)
	*/
	IElement* GetElement(const std::string& s);
}