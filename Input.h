#pragma once
#include "dinput.h"

class Input
{
private:
	BYTE keys[256];
	BYTE oldKeys[256];

	IDirectInput8 * directInput;
	IDirectInputDevice8 * keyboard;

public:
	// コンストラクタ
	Input();

	// デストラクタ
	~Input();

	// 初期化
	void InputInitialize(WNDCLASSEX w, HWND hwnd);

	// 更新処理
	void InputUpdate();
	bool KeyDown(char key);
	bool KeyTrigger(char key);
};