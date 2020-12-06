#pragma once

#include "Managers/DataManager.h"
#include "Gui/IElem.h"
#include "Gui/IButton.h"
#include "Gui/ISquare.h"
#include "Gui/IInput.h"
#include "Gui/WorldItem.h"
#include <unordered_map>
//#include "Gui/ServerItem.h"

/*
Global Manager for 2D elements (buttons, text)
 The iManager stores these elements in a global acceccible vector
 Two types of rendering (start menu buttons, in game inventories)
*/
namespace iManager {
	
	bool IsPauseMode();
	void SetPauseMode(bool f);
	/*
	Setup callbacks and 2D elements
	*/
	void Init();
	void UpdateInterface(float delta);
	void RenderInterface();

	void AddElement(IElem* e);
	IElem* GetElement(std::string s);

	Font* GetFont();
}