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
public:
	// �Ή����x���̔z��
	D3D_FEATURE_LEVEL levels[4];

	ID3D12Device * device = nullptr;
	IDXGIFactory7 * dxgiFactory = nullptr;
	IDXGISwapChain4 * swapChain = nullptr;
	ID3D12CommandAllocator * cmdAllocator = nullptr;
	ID3D12GraphicsCommandList * commandList = nullptr;
	ID3D12CommandQueue * commandQueue = nullptr;
	ID3D12DescriptorHeap * rtvHeap = nullptr;

	// �f�X�N���v�^�q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};

	// �o�b�N�o�b�t�@
	std::vector<ID3D12Resource *> backBuffers;

	// �t�F���X
	ID3D12Fence * fence = nullptr;
	UINT64 fenceVal = 0;

	// �R���X�g���N�^
	DirectXManager();

	// �f�X�g���N�^
	~DirectXManager();

	// DirectX�̏�����
	void DirectXInitialize(HWND & hwnd);
};