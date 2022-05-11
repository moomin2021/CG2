#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#include <vector>
#include <string>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class DirectXManager
{
private:
	IDXGIFactory7 * dxgiFactory;

	// ここに特定の名前を持つアダプターオブジェクトが入る
	IDXGIAdapter4 * tmpAdapter;

	// 対応レベルの配列
	D3D_FEATURE_LEVEL levels[4];

	D3D_FEATURE_LEVEL featureLevel;

	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;

public:
	HRESULT result;
	ID3D12Device * device;

	IDXGISwapChain4 * swapChain;
	ID3D12CommandAllocator * cmdAllocator;
	ID3D12GraphicsCommandList * commandList;
	ID3D12CommandQueue * commandQueue;
	ID3D12DescriptorHeap * rtvHeap;

	std::vector<ID3D12Resource *> backBuffers;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	ID3D12Fence * fence;
	UINT64 fenceVal;

	// コンストラクタ
	DirectXManager();

	// デストラクタ
	~DirectXManager();

	// DirectXの初期化
	void DirectXInitialize(HWND hwnd);
};