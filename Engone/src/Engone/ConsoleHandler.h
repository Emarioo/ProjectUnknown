#pragma once

namespace console
{
	enum class Color : uint8_t
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
	Color operator|(Color a, Color b);

	//-- Console settings
	void Init(int w, int h);
	void Allocate();
	void SetSize(int w, int h);
	void SetTitle(const std::string& str);

	//-- Cursor
	void SetCursorBlinking(bool f);
	// Not handling out of bounds
	void SetCursor(int x, int y);

	//-- Get stuff
	void GetConsoleInput(std::string& str);
	int GetWidth();
	int GetHeight();

	//-- Console Drawing
	/*
	Not handling out of bounds, chr=0 will only change the color
	*/
	void Fill(int x, int y, int w, int h, char chr, Color color);
	// chr=0 will only change the color
	void Fill(int x, int y, char chr, Color color);
	void Print(int x, int y, const std::string& str, Color color= Color::WHITE);
	void PrintCenter(int x, int y, const std::string& str, Color color = Color::WHITE);
	// Will clear screen
	void Clear(Color color);
	void Render();

}