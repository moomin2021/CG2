#pragma once
#include "Window.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#include <vector>
#include <string>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class DirectXManager
{
public:
	// 対応レベルの配列
	D3D_FEATURE_LEVEL levels[4];

	ID3D12Device * device = nullptr;
	IDXGIFactory7 * dxgiFactory = nullptr;
	IDXGISwapChain4 * swapChain = nullptr;
	ID3D12CommandAllocator * cmdAllocator = nullptr;
	ID3D12GraphicsCommandList * commandList = nullptr;
	ID3D12CommandQueue * commandQueue = nullptr;
	ID3D12DescriptorHeap * rtvHeap = nullptr;

	// デスクリプタヒープ
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;

	// バックバッファ
	std::vector<ID3D12Resource *> backBuffers;

	// フェンス
	ID3D12Fence * fence = nullptr;
	UINT64 fenceVal = 0;

	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView;

	// パイプランステート
	ID3D12PipelineState * pipelineState;

	// ルートシグネチャ
	ID3D12RootSignature * rootSignature;

	// インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW ibView;

	ID3D12Resource * constBuffMaterial;

	ID3D12DescriptorHeap * srvHeap;

	ID3D12Resource * constBuffTransform;

	// インデックスデータ
	unsigned short indices[6] =
	{
		0, 1, 2,// 三角形1つ目
		1, 2, 3,// 三角形2つ目
	};

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	DirectXManager();

	// デストラクタ
	~DirectXManager();

	/// <summary>
	/// DirectXの初期化処理
	/// </summary>
	/// <param name="hwnd"> ウィンドウハンドル型 </param>
	void DirectXInitialize(HWND & hwnd);

	/// <summary>
	/// 描画初期化処理
	/// </summary>
	/// <param name="winWidth"> ウィンドウ横幅 </param>
	/// <param name="winHeight"> ウィンドウ縦幅 </param>
	void DrawInitialize(int winWidth, int winHeight);

	/// <summary>
	/// DirectXの毎フレーム処理
	/// </summary>
	void DirectXUpdate(int winWidth, int winHeight);
};