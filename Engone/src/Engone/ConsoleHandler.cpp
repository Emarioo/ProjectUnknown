#include "gonpch.h"

#include "ConsoleHandler.h"

namespace console
{
	Color operator|(Color a, Color b)
	{
		return (Color)((uint8_t)a | (uint8_t)b);
	}

	static HANDLE inHandle;
	static HANDLE outHandle;
	static INPUT_RECORD inRecord[20];
	static DWORD numRead;

	static CHAR_INFO* consoleBuffer;
	static int width, height;

	static bool needUpdate=false;

	void Init(int w, int h)
	{
		inHandle = GetStdHandle(STD_INPUT_HANDLE);
		outHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		// create window
		//DWORD mode;
		//GetConsoleMode(inHandle, &mode);
		//std::cout << mode<<"\n";

		//mode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;
		//SetConsoleMode(inHandle,mode);

		SetCursorBlinking(false);

		SetSize(w, h);
	}
	void Allocate()
	{
		bool b = AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
		HANDLE oHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		HANDLE iHandle = GetStdHandle(STD_INPUT_HANDLE);
	}
	void SetSize(int w, int h)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(outHandle, &csbi);

		COORD coord = { w,h, };
		if (!SetConsoleScreenBufferSize(outHandle, coord)) {
			//std::cout << "No1\n";
		}

		SMALL_RECT rect = { 0,0,w - 1,h - 1 };
		if (!SetConsoleWindowInfo(outHandle, TRUE, &rect)) {
			//std::cout << "No\n";
		}

		width = w;
		height = h;

		if (consoleBuffer != nullptr)
			delete[] consoleBuffer;

		consoleBuffer = new CHAR_INFO[w * h];
		for (int i = 0; i < w * h; i++) {
			consoleBuffer[i].Attributes = 0;
			consoleBuffer[i].Char.AsciiChar = 0;
			consoleBuffer[i].Char.UnicodeChar = 0;
		}
		needUpdate = true;
	}
	void SetCursorBlinking(bool f)
	{
		CONSOLE_CURSOR_INFO info;
		GetConsoleCursorInfo(outHandle, &info);
		info.bVisible = f;
		SetConsoleCursorInfo(outHandle, &info);
	}
	void GetConsoleInput(std::string& str)
	{
		// Needs fixing
		SetCursorBlinking(true);
		std::getline(std::cin, str);
		SetCursorBlinking(false);
	}
	void SetTitle(const std::string& str)
	{
		TCHAR* title = new TCHAR[str.length() + 1];
		for (int i = 0; i < str.length(); i++)
			title[i] = str[i];
		title[str.length()] = '\0';
		SetConsoleTitle(title);
		delete[] title;
	}
	/*
	void GetWidthAndHeight(int* x, int* y)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (GetConsoleScreenBufferInfo(outHandle, &csbi)) {
			if (x != nullptr)
				*x = csbi.dwSize.X;
			if (y != nullptr)
				*y = csbi.dwSize.Y;
			return;
		}
		else
		{
			if (x != nullptr)
				*x = -1;
			if (y != nullptr)
				*y = -1;
		}
	}*/
	int GetWidth()
	{
		return width;
		/*
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (GetConsoleScreenBufferInfo(outHandle, &csbi)) {
			return csbi.dwSize.X;
		}
		else
			return -1;*/
	}
	int GetHeight()
	{
		return height;/*
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (GetConsoleScreenBufferInfo(outHandle, &csbi)) {
			return csbi.dwSize.Y;
		}
		else
			return -1;*/
	}
	void SetCursor(int x, int y)
	{
		SetConsoleCursorPosition(outHandle, { (short)x,(short)y });
	}
	void Fill(int x, int y, int w, int h, char chr, Color color)
	{
		if (consoleBuffer == nullptr)
			return;
		for (int i = 0; i < w * h; i++) {
			if (y + i / w<0 || y + i / w>height - 1 ||
				x + i % w<0 || x + i % w>width - 1)
				continue;
			if(chr!=0)
				consoleBuffer[(y + i / w) * width + x + i % w].Char.UnicodeChar = chr;
			consoleBuffer[(y + i / w) * width + x + i % w].Attributes = (int)color;
		}
		needUpdate = true;
		/*
		COORD coord = { x,y };
		DWORD count;
		CONSOLE_SCREEN_BUFFER_INFO csbi;

		if (GetConsoleScreenBufferInfo(outHandle, &csbi)) {
			if (w == csbi.dwSize.X) {
				FillConsoleOutputCharacter(outHandle, chr, csbi.dwSize.X * h, coord, &count);
				FillConsoleOutputAttribute(outHandle, (WORD)color, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
			}
			else {
				for (int i = 0; i < h; i++) {
					coord.Y = y + i;
					FillConsoleOutputCharacter(outHandle, chr, w, coord, &count);
					FillConsoleOutputAttribute(outHandle, (WORD)color, w, coord, &count);
				}
			}
		}*/
	}
	void Fill(int x, int y, char chr, Color color)
	{
		if (consoleBuffer == nullptr)
			return;
		if (y<0 || y>height - 1 || x <0 || x >width - 1)
			return;
		consoleBuffer[y * width + x].Char.UnicodeChar = chr;
		consoleBuffer[y * width + x].Attributes = (int)color;
		needUpdate = true;
	}
	void Print(int x, int y, const std::string& str, Color color)
	{
		if (consoleBuffer == nullptr)
			return;
		if (y<0 || y>height - 1)
			return;
		for (int i = 0; i < str.length(); i++) {
			if (x + i<0 || x + i>width - 1)
				continue;
			consoleBuffer[y * width + x + i].Char.UnicodeChar = str[i];
			consoleBuffer[y * width + x + i].Attributes = (int)color;
		}
		needUpdate = true;
		/*
		LPWSTR nuisance;
		nuisance = new WCHAR[str.length()];
		for (int i = 0; i < str.length(); i++) {
			nuisance[i] = str[i];
		}
		DWORD num;
		COORD coord = { x,y };
		WriteConsoleOutputCharacter(outHandle, nuisance, str.length(), coord, &num);
		FillConsoleOutputAttribute(outHandle, (WORD)color, str.length(), coord, &num);

		delete[] nuisance;
		*/
	}
	void PrintCenter(int x, int y, const std::string& str, Color color)
	{
		if (consoleBuffer == nullptr)
			return;
		if (y<0 || y>height - 1)
			return;
		for (int i = 0; i < str.length(); i++) {
			if (x + i - str.length() / 2 < 0 || x + i - str.length() / 2 > width - 1)
				continue;
			consoleBuffer[y * width + x + i - str.length() / 2].Char.UnicodeChar = str[i];
			consoleBuffer[y * width + x + i - str.length() / 2].Attributes = (int)color;
		}
		needUpdate = true;
		/*
		LPWSTR nuisance;
		nuisance = new WCHAR[str.length()];
		for (int i = 0; i < str.length(); i++) {
			nuisance[i] = str[i];
		}
		DWORD num;
		COORD coord = { x-str.length()/2,y };
		WriteConsoleOutputCharacter(outHandle, nuisance, str.length(), coord, &num);
		FillConsoleOutputAttribute(outHandle, (WORD)color, str.length(), coord, &num);

		delete[] nuisance;*/
	}
	void Render()
	{
		if (consoleBuffer == nullptr)
			return;
		if (needUpdate) {
			needUpdate = false;
			COORD s = { width,height };
			COORD p = { 0,0 };
			SMALL_RECT r = { 0,0,width,height };
			WriteConsoleOutput(outHandle, consoleBuffer, s, p, &r);
		}
	}
	void Clear(Color color)
	{
		Fill(0, 0, width, height, ' ', color);
	}
}