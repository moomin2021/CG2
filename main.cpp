#include <Windows.h>

// Windowsアプリでエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	// コンソールへの文字出力
	OutputDebugStringA("Hello, DirectX!!\n");

	return 0;

}