#include "Input.h"
#include "Window.h"
#include "DirectXManager.h"

// Windowsアプリでエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウのクラス生成
	Window * win = new Window(1280, 720);

	// ウィンドウの初期化
	win->WindowInitialize();

	// DirectX管理クラス生成
	DirectXManager * directXM = new DirectXManager();

	// DirectXの初期化
	directXM->DirectXInitialize(win->hwnd);
	// Inputクラス生成
	Input * input = new Input();

	// キーボード情報の初期化
	input->InputInitialize(win->w, win->hwnd);

	// 描画初期化処理
	directXM->DrawInitialize(win->GetWindowWidth(), win->GetWindowHeight());

	// ゲームループ
	while (true)
	{
		// メッセージがある？
		if (PeekMessage(&win->msg, nullptr, 0, 0, PM_REMOVE))
		{
			// キー入力メッセージ処理
			TranslateMessage(&win->msg);

			// プロシージャにメッセージを送る
			DispatchMessage(&win->msg);
		}

		// ×ボタンで終了メッセージが来たらゲームループを抜ける
		if (win->msg.message == WM_QUIT)
		{
			delete win;
			break;
		}

		// DirectX毎フレーム処理　ここから

#pragma region

		// キーボードの更新処理
		input->InputUpdate();

		// DirectX毎フレーム処理
		directXM->DirectXUpdate(win->GetWindowWidth(), win->GetWindowHeight());

#pragma endregion

		// DirectX毎フレーム処理　ここまで
	}

	return 0;
}