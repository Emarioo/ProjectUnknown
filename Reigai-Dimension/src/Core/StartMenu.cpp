#include "StartMenu.h"

namespace startMenu {

	bool menu = true;
	bool dplay = true;
	bool notStart = false;
	void Init() {
		using namespace iManager;

		ISquare* squ = new ISquare("background", "0p,0p,2.0,2.0", "noise");
		squ->AddTag(&menu);
		//AddElement(squ);

		IButton* but = new IButton("playAlone","-740p,400p,400p,200p",0.4,0.85,0.4,1, [&]() {
			std::cout << "Play Alone" << std::endl;
		}, 0.15);
		but->Text("Play Alone",GetFont(),1.0,1.0,1.0,1);
		but->Click.Fade(0.5,0.5,0.7,1,0.15);
		but->Hover.Fade(0.7, 0.7, 0.9, 1, 0.5);
		but->Hover.Size("100p,20p",0.4);
		but->Hover.Move("50p,0p",0.4);
		but->AddTag(&menu);
		IAction *a = but->NewAction(&notStart);
		a->Move("-400p,0.0",0.5);
		AddElement(but);

		but = new IButton("joinFriend", "-740p,180p,400p,200p", 0.4, 0.85, 0.4, 1, [&]() {
			std::cout << "Join Friend" << std::endl;
		}, 0.15);
		but->Text("Join a Friend", GetFont(), 1, 1, 1, 1);
		but->Click.Fade(0.5, 0.5, 0.7, 1, 0.15);
		but->Hover.Fade(0.7, 0.7, 0.9, 1, 0.5);
		but->Hover.Size("100p,20p", 0.4);
		but->Hover.Move("50p,0p", 0.4);
		but->AddTag(&menu);
		a = but->NewAction(&notStart);
		a->Move("-400p,0.0", 0.5);
		AddElement(but);
		
		but = new IButton("mainSettings", "-740p,-40p,400p,200p", 0.4, 0.85, 0.4, 1, [&]() {
			std::cout << "Settings" << std::endl;
			dplay = !dplay;
		}, 0.15);
		but->Text("Settings", GetFont(), 1, 1, 1, 1);
		but->Click.Fade(0.5, 0.5, 0.7, 1, 0.15);
		but->Hover.Fade(0.7, 0.7, 0.9, 1, 0.5);
		but->Hover.Size("100p,20p", 0.4);
		but->Hover.Move("50p,0p", 0.4);
		but->AddTag(&menu);
		a = but->NewAction(&notStart);
		a->Move("-400p,0.0", 0.5);
		AddElement(but);

		but = new IButton("mainQuit", "-740p,-260p,400p,200p", 0.4, 0.85, 0.4, 1, [&]() {
			std::cout << "Quit" << std::endl;
			dplay = !dplay;
		}, 0.15);
		but->Text("Quit", GetFont(), 1, 1, 1, 1);
		but->Click.Fade(0.5, 0.5, 0.7, 1, 0.15);
		but->Hover.Fade(0.7, 0.7, 0.9, 1, 0.5);
		but->Hover.Size("100p,20p", 0.4);
		but->Hover.Move("50p,0p", 0.4);
		but->AddTag(&menu);
		a = but->NewAction(&notStart);
		a->Move("-400p,0.0", 0.5);
		AddElement(but);
	}
}