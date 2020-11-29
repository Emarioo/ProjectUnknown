#pragma once

#include "RefManager.h"
#include "Gui/WorldItem.h"
//#include "Gui/ServerItem.h"

namespace iManager {

	bool GetPauseMode();
	void SetPauseMode(bool f);
	/*
	Setup callbacks and 2D elements
	*/
	void Init();
	void UpdateInterface();
	void RenderInterface();

	void AddElement(IElem e);
	IElem* GetElement(std::string s);

	Font* GetFont();
}