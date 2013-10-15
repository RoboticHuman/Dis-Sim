#include <windows.h>
#include <iostream>

using namespace std;

#ifndef COLORCMD_H
#define COLORCMD_H

class TextAttr 
{
	friend std::ostream& operator<<(std::ostream& out, TextAttr attr)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attr.value);
			return out;
	}
	public:
	explicit TextAttr(WORD attributes): value(attributes) {}
	private:
		WORD value;
};
#define FOREGROUND_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define Cwhite	TextAttr(FOREGROUND_INTENSITY | FOREGROUND_WHITE)
#define Cred	TextAttr(FOREGROUND_INTENSITY | FOREGROUND_RED)
#define Cgreen	TextAttr(FOREGROUND_INTENSITY | FOREGROUND_GREEN)
#define Cblue	TextAttr(FOREGROUND_INTENSITY | FOREGROUND_BLUE)

#endif