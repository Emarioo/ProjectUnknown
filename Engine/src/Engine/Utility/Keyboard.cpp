#include "Keyboard.h"

#include <Windows.h>
#include <iostream>
#include <vector>
#include "Rendering/Renderer.h"

namespace engine {

	/*
	https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-lcid/70feba9f-294e-491e-b6eb-56532684c37f
	file:///C:/Users/emarioo/Downloads/[MS-LCID].pdf
	*/

	std::vector<std::string> languages{
	"041D Sweden",
	"0409 United States",
	"0809 United Kingdom"
	};

	// order:
	// 39,44-47,48-57,59,61,91-93,96,161,162
	const char langs[3][49]{
		{132,',','+','.','-','0','1','2','3','4','5','6','7','8','9',148,239,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',134,'\'',249,246,' ','<'},
		{'\'',',','-','.','/','0','1','2','3','4','5','6','7','8','9',';','=','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','[','\\',']','`',' ','\\'},
		{'\'',',','-','.','/','0','1','2','3','4','5','6','7','8','9',';','=','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','[','#',']','`',' ','\\'},
	};
	const char slangs[3][49]{
		{142,';','?',':','_','=','!','"','#',207,'%','&','/','(',')',153,'`','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',143,'*','^',171,' ','>'},
		{'"','<','_','>','?',')','!','@','#','$','%','^','&','*','(',':','=','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','{','|','}','~',' ','|'},
		{'@','<','_','>','?',')','!','\"','£','$','%','^','&','*','(',':','+','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','{','~','}','¬',' ','|'},
	};
	const int alangs[3][49]{
		{' ',' ','\\',' ',' ','}',' ','@',156,'$','€',' ','{','[',']',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','~',' ',' ',179},
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{' ',' ',' ',' ',' ',' ',' ',' ',' ','€',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\\',' ','¦',' ',' '},
	};

	int lang = 0;
	void UpdateKeyboard(bool l) {
		TCHAR board[KL_NAMELENGTH];
		//GetKeyboardLayoutNameA(board); NOTE: Removed A because of error in VS 2019 compared to VS 2017
		GetKeyboardLayoutName(board);
		std::string c;
		for (int i = 0; i < 4; i++) {
			c += board[i + 4];
		}
		for (int i = 0; i < languages.size(); i++) {
			if (languages[i].substr(0, 4) == c) {
				lang = i;
				break;
			}
		}
		if (l)
			std::cout << "Using " << languages[lang].substr(5).c_str() << "'s Keyboard Layout" << std::endl;
		if (0) {
			for (int i = -128; i < 128; i++) {
				std::cout << (char)(i + 128) << " ";
			}
		}
		if (0) {
			for (int i = 0; i < 16; i++) {
				for (int j = 0; j < 16; j++) {
					std::cout << (char)(i * 16 + j) << "=" << (i * 16 + j) << " ";
				}
				std::cout << std::endl;
			}
		}

	}
	// Return -128 to 127
	//  0 for unkown character
	// TODO: Support for keypad?
	char GetChar(int k, bool shift, bool alt) {
		if (shift) {
			if (k == 32) {// Space
				return 32;
			} else if (k == 39) {// Apostrophe
				return slangs[lang][k - 39];
			} else if (k >= 44 && k <= 47) {// Comma, Minus, Period, Slash
				return slangs[lang][k - 44 + 1];
			} else if (k >= 48 && k <= 57) {// Numbers
				return slangs[lang][k - 48 + 5];
			} else if (k == 59) {// Semicolon
				return slangs[lang][k - 59 + 15];
			} else if (k == 61) {// Equal
				return slangs[lang][k - 61 + 16];
			} else if (k >= 65 && k <= 90) {// A-Z
				return slangs[lang][k - 65 + 17];
			} else if (k >= 91 && k <= 93) {// Left Bracket, Backslash, Right Bracket
				return slangs[lang][k - 91 + 43];
			} else if (k == 96) {// Grave Accent
				return slangs[lang][k - 96 + 46];
			} else if (k >= 161 && k <= 162) {// World 1 & 2
				return slangs[lang][k - 161 + 47];
			}
		} else if (alt) {
			/*if (k == 32) { // Space is disabled
				return = ' ';
			} else */if (k == 39) {// Apostrophe
				return alangs[lang][k - 39];
			} else if (k >= 44 && k <= 47) {// Comma, Minus, Period, Slash
				return alangs[lang][k - 44 + 1];
			} else if (k >= 48 && k <= 57) {// Numbers
				return alangs[lang][k - 48 + 5];
			} else if (k == 59) {// Semicolon
				return alangs[lang][k - 59 + 15];
			} else if (k == 61) {// Equal
				return alangs[lang][k - 61 + 16];
			} else if (k >= 65 && k <= 90) {// A-Z
				return alangs[lang][k - 65 + 17];
			} else if (k >= 91 && k <= 93) {// Left Bracket, Backslash, Right Bracket
				return alangs[lang][k - 91 + 43];
			} else if (k == 96) {// Grave Accent
				return alangs[lang][k - 96 + 46];
			} else if (k >= 161 && k <= 162) {// World 1 & 2
				return alangs[lang][k - 161 + 47];
			}
		} else {
			if (k == 32) {// Space
				return ' ';
			} else if (k == 39) {// Apostrophe
				return langs[lang][k - 39];
			} else if (k >= 44 && k <= 47) {// Comma, Minus, Period, Slash
				return langs[lang][k - 44 + 1];
			} else if (k >= 48 && k <= 57) {// Numbers
				return langs[lang][k - 48 + 5];
			} else if (k == 59) {// Semicolon
				return langs[lang][k - 59 + 15];
			} else if (k == 61) {// Equal
				return langs[lang][k - 61 + 16];
			} else if (k >= 65 && k <= 90) {// A-Z
				return langs[lang][k - 65 + 17];
			} else if (k >= 91 && k <= 93) {// Left Bracket, Backslash, Right Bracket
				//std::cout << "This key " << (int)langs[lang][k - 91 + 43] << std::endl;
				return langs[lang][k - 91 + 43];
			} else if (k == 96) {// Grave Accent
				return langs[lang][k - 96 + 46];
			} else if (k >= 161 && k <= 162) {// World 1 & 2
				return langs[lang][k - 161 + 47];
			}
		}
		return 0;
	}
	bool IsKey(int key) {
		return glfwGetKey(GetWindow(),key)==GLFW_PRESS;
	}
}