#include "Input.h"
#include <cassert>

// DirectInputのバージョン指定
#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

static IDirectInput8 * directInput = nullptr;
static IDirectInputDevice8 * keyboard = nullptr;

static BYTE keys[256] = {};
static BYTE oldKeys[256] = {};

void Input::InputInitialize(HRESULT result, WNDCLASSEX w, HWND hwnd)
{
	// DirectInputの初期化
	result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void **)&directInput, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	// 入力データの形跡セット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::InputUpdate()
{
	// キーボード情報の取得開始
	keyboard->Acquire();

	for (int i = 0; i < 256; i++)
	{
		oldKeys[i] = keys[i];
	}

	keyboard->GetDeviceState(sizeof(keys), keys);
}

bool Input::KeyDown(char key)
{
	return keys[key];
}

bool Input::KeyTrigger(char key)
{
	return keys[key] && !oldKeys[key];
}