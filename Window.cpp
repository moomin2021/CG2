#include "Window.h"

// コンストラクタ
Window::Window() :
	windowWidth(1280), windowHeight(720), w{}, wrc{}, hwnd{}, msg{}
{

}


// デストラクタ
Window::~Window()
{
	UnregisterClass(w.lpszClassName, w.hInstance);
}

// ウィンドウプロシージャ
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		// ウィンドウが破棄された
		case WM_DESTROY:

			// OSに対して、アプリの終了を伝える
			PostQuitMessage(0);

			return 0;
	}
	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

// ウィンドウの初期化
void Window::WindowInitialize()
{
	// ウィンドウクラスの設定
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc; // ----> ウィンドウプロシージャを設定
	w.lpszClassName = L"DirectXGame"; // -------> ウィンドウクラス名
	w.hInstance = GetModuleHandle(nullptr);// --> ウィンドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW);// -> カーソル指定

		// ウィンドウクラスをOSに登録する
	RegisterClassEx(&w);

	// ウィンドウサイズ{ X座標 Y座標 横幅 縦幅 }
	wrc = { 0, 0, windowWidth, windowHeight };

	// 自動でサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウオブジェクトの生成
	hwnd = CreateWindow(
		w.lpszClassName, // ------> クラス名
		L"DirectXGame", // -------> タイトルバーの文字
		WS_OVERLAPPEDWINDOW,// ---> 標準的なウィンドウスタイル
		CW_USEDEFAULT, // --------> 表示X座標（OSに任せる）
		CW_USEDEFAULT, // --------> 表示Y座標（OSに任せる）
		wrc.right - wrc.left, // -> ウィンドウ横幅
		wrc.bottom - wrc.top, // -> ウィンドウ縦幅
		nullptr, // --------------> 親ウィンドウハンドル
		nullptr, // --------------> メニューハンドル
		w.hInstance, // ----------> 呼び出しアプリケーションハンドル
		nullptr); // -------------> オプション

	// ウィンドウを表示状態にする
	ShowWindow(hwnd, SW_SHOW);
}

// ウィンドウの横幅を返す
int Window::GetWindowWidth()
{
	return windowWidth;
}

// ウィンドウの縦幅を返す
int Window::GetWindowHeight()
{
	return windowHeight;
}