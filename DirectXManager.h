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

	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	IDXGIAdapter4 * tmpAdapter;

	// �Ή����x���̔z��
	D3D_FEATURE_LEVEL levels[4];

	D3D_FEATURE_LEVEL featureLevel;

	//�R�}���h�L���[�̐ݒ�
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

	// �R���X�g���N�^
	DirectXManager();

	// �f�X�g���N�^
	~DirectXManager();

	// DirectX�̏�����
	void DirectXInitialize(HWND hwnd);
};