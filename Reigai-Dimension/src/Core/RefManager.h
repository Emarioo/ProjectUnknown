#pragma once

#include "Rendering/Renderer.h"
#include "Objects/GameObject.h"
#include "World/Dimension.h"
#include "Gui/IElem.h"
#include "Objects/ClientPlayer.h"
#include "Data/DataManager.h"

namespace rManager {
	enum Menu {
		Startup,
		Gameplay
	};
	Menu GetMenu();
	void SetMenu(Menu);

	ClientPlayer& GetPlayer();
	/*
	// void JoinDimension(); ?
	void AddDimension(std::string,Dimension);
	/*
	Can return nullptr
	*//*
	Dimension* GetDimension();
	void SetDimension(std::string);
	*/
	/*
	tex or color
	*/
	void AddObject(const std::string&, GameObject&);
	void DelObject(const std::string&, GameObject&);
	/*
	void AddRenderO(std::string, GameObject*);
	void DelRenderO(std::string, GameObject*);

	void AddUpdateO(std::string, GameObject&);
	void DelUpdateO(std::string, GameObject&);
	*/
	/*
	std::vector<IElem>* GetElements();
	IElem* AddElement();
	void DelElement(IElem*);
	*/
	void RenderGame();
	//void UpdateInterface();
	//void RenderInterface();

	void Init();

	/*
	void AddElement();
	void DelElement();
	*/
}