#pragma once

#include "Rendering/Renderer.h"
#include "Objects/GameObject.h"
#include "World/Dimension.h"
#include "Gui/IElem.h"
#include "Objects/ClientPlayer.h"

enum Menu {
	Startup,
	Gameplay
};
Menu GetMenu();
void SetMenu(Menu);

ClientPlayer* GetPlayer();

// void JoinDimension(); ?
void AddDimension(std::string,Dimension);
/*
Can return nullptr
*/
Dimension* GetDimension();
void SetDimension(std::string);

void AddRenderO(std::string, GameObject*);
void DelRenderO(std::string, GameObject*);
/*
std::vector<IElem>* GetElements();
IElem* AddElement();
void DelElement(IElem*);
*/
void RenderGame();
//void UpdateInterface();
//void RenderInterface();

void InitManager();

void AddHitbox(std::string,std::string);
Collision* GetHitbox(std::string);

/*
void AddElement();
void DelElement();
*/