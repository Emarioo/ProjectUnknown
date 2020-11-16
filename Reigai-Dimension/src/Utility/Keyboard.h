#pragma once

// The point of this class is to convert numbers to char, string and the other way around
//  Basically an intepreter for key codes

void UpdateKeyboard(bool l);// Check current keyboard layout and change intepreter
char GetChar(int c, bool shift, bool alt);
