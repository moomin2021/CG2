#pragma once
#include "Windows.h"

class Window
{
private:
	// ウィンドウサイズ
	const int windowWidth;// --> 横
	const int windowHeight;// -> 縦

public:
	//ウィンドウクラス
	WNDCLASSEX w;

	// ウィンドウサイズ
	RECT wrc;

	// ウィンドウオブジェクト
	HWND hwnd;

	// メッセージ
	MSG msg;

	// コンストラクタ
	Window(int width, int height);

	// デストラクタ
	~Window();

	// ウィンドウの初期化
	void WindowInitialize();

	// ウィンドウの横幅を返す
	int GetWindowWidth();

	// ウィンドウの縦幅を返す
	int GetWindowHeight();
};