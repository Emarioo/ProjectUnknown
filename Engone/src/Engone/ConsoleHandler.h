#pragma once

namespace console
{
	enum class ConsoleColor : uint8_t
	{
		BLACK = 0,
		NAVY,
		GREEN,
		TEAL,
		BLOOD,// blood
		MAGENTA,
		GOLD,
		SILVER,
		GRAY,
		BLUE,
		LIME,
		AQUA,
		RED,
		PINK,
		YELLOW,
		WHITE,
		_BLACK = 0x00,
		_NAVY = 0x10,
		_GREEN = 0x20,
		_TEAL = 0x30,
		_BLOOD = 0x40,
		_MAGENTA = 0x50,
		_GOLD = 0x60,
		_SILVER = 0x70,
		_GRAY = 0x80,
		_BLUE = 0x90,
		_LIME = 0xa0,
		_AQUA = 0xb0,
		_RED = 0xc0,
		_PINK = 0xd0,
		_YELLOW = 0xe0,
		_WHITE = 0xf0,
	};
	ConsoleColor operator|(ConsoleColor a, ConsoleColor b);

	//-- Console settings
	void InitConsole(int w, int h);
	void SetConsoleSize(int w, int h);
	void SetTitleConsole(const std::string& str);

	//-- Cursor
	void SetConsoleCursorBlinking(bool f);
	// Not handling out of bounds
	void SetConsoleCursor(int x, int y);

	//-- Get stuff
	void GetConsoleInput(std::string& str);
	int GetWidth();
	int GetHeight();

	//-- Console Drawing
	/*
	Not handling out of bounds, chr=0 will only change the color
	*/
	void FillConsole(int x, int y, int w, int h, char chr, ConsoleColor color);
	// chr=0 will only change the color
	void FillConsole(int x, int y, char chr, ConsoleColor color);
	void ConsoleString(int x, int y, const std::string& str, ConsoleColor color);
	void ConsoleCenterString(int x, int y, const std::string& str, ConsoleColor color);
	// Will clear screen
	void ClearConsole(ConsoleColor color);
	void RenderConsole();

}