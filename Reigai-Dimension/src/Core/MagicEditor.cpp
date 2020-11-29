#include "MagicEditor.h"

/*
Current script,
*/

void EditorCompile() {
	// Compile current
	std::cout << "Compile" << std::endl;
}

void InitEditor() {
	
	IElem back("editor_back");
	back.AddTag(8);
	back.Dimi(0, 0, 1500, 800);
	back.Tex(0.3, 0.3, 0.3, 0.99);
	iManager::AddElement(back);

	// Load scripts
	IElem script("editor_script");
	script.AddTag(8);
	script.Dimi(0, 0, 1480, 780);
	script.Tex(0.2, 0.2, 0.2, 0.99);
	script.Text(500, iManager::GetFont(), 0.7, 0.7, 0.7, 1);
	script.SetText("500 charsgd gkkgd\n mjp");
	script.Typing();
	iManager::AddElement(script);

	IElem comp("editor_compile");
	comp.AddTag(8);
	comp.Dimi(690, -365, 100, 50);
	comp.Tex(0.4, 0.4, 0.4, 0.99);
	comp.Text("Compile", iManager::GetFont(), 0.7, 0.7, 0.7, 1);
	IAction* a = &comp.ClickEvent;
	a->Fade(0.2, 0.3, 0.6, 1, 0.1);
	a->Func(EditorCompile, 0.15);
	iManager::AddElement(comp);
}