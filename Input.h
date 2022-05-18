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
	// �R���X�g���N�^
	Input();

	// �f�X�g���N�^
	~Input();

	// ������
	void InputInitialize(WNDCLASSEX w, HWND hwnd);

	// �X�V����
	void InputUpdate();
	bool KeyDown(char key);
	bool KeyTrigger(char key);
};