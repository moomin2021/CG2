#pragma once
#include "dinput.h"

class Input
{
private:
	UINT8 keys[256];
	UINT8 oldKeys[256];

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
	bool KeyDown(UINT8 key);
	bool KeyTrigger(UINT8 key);
};