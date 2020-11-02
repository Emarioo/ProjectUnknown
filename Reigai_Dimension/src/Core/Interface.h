#pragma once

#include "RefsManager.h"
#include "Gui/WorldItem.h"
//#include "Gui/ServerItem.h"

bool GetPauseMode();
void SetPauseMode(bool f);
void InitInterface();
void UpdateInterface();
void RenderInterface();

void AddElement(IElem e);
IElem* GetElement(std::string s);

Font* GetFont();