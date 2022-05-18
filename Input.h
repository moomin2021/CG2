#pragma once
#include "dinput.h"

class Input
{
public:
	static void InputInitialize(WNDCLASSEX w, HWND hwnd);
	static void InputUpdate();
	static bool KeyDown(char key);
	static bool KeyTrigger(char key);
};