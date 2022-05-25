#include "Input.h"
#include <cassert>

// DirectInput�̃o�[�W�����w��
#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Input::Input() : keys{}, oldKeys{}, directInput{}, keyboard{}
{

}

void Input::InputInitialize(WNDCLASSEX w, HWND hwnd)
{
	HRESULT result{};

	// DirectInput�̏�����
	result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void **)&directInput, nullptr);
	assert(SUCCEEDED(result));

	// �L�[�{�[�h�f�o�C�X�̐���
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	// ���̓f�[�^�̌`�ՃZ�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // �W���`��
	assert(SUCCEEDED(result));

	// �r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::InputUpdate()
{
	// �L�[�{�[�h���̎擾�J�n
	keyboard->Acquire();

	for (int i = 0; i < 256; i++)
	{
		oldKeys[i] = keys[i];
	}

	keyboard->GetDeviceState(sizeof(keys), keys);
}

bool Input::KeyDown(UINT8 key)
{
	return keys[key];
}

bool Input::KeyTrigger(UINT8 key)
{
	return keys[key] && !oldKeys[key];
}