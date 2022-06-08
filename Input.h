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
	// �R���X�g���N�^
	Input();

	// �f�X�g���N�^
	~Input();

	// ������
	void InputInitialize(WNDCLASSEX w, HWND hwnd);

	// �X�V����
	void InputUpdate();
	bool PushKey(UINT8 key);
	bool TriggerKey(UINT8 key);
};