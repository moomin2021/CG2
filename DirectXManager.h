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
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;

	// �o�b�N�o�b�t�@
	std::vector<ID3D12Resource *> backBuffers;

	// �t�F���X
	ID3D12Fence * fence = nullptr;
	UINT64 fenceVal = 0;

	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView;

	// �p�C�v�����X�e�[�g
	ID3D12PipelineState * pipelineState;

	// ���[�g�V�O�l�`��
	ID3D12RootSignature * rootSignature;

	// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	D3D12_INDEX_BUFFER_VIEW ibView;

	ID3D12Resource * constBuffMaterial;

	ID3D12DescriptorHeap * srvHeap;

	ID3D12Resource * constBuffTransform;

	// �C���f�b�N�X�f�[�^
	unsigned short indices[6] =
	{
		0, 1, 2,// �O�p�`1��
		1, 2, 3,// �O�p�`2��
	};

public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	DirectXManager();

	// �f�X�g���N�^
	~DirectXManager();

	/// <summary>
	/// DirectX�̏���������
	/// </summary>
	/// <param name="hwnd"> �E�B���h�E�n���h���^ </param>
	void DirectXInitialize(HWND & hwnd);

	/// <summary>
	/// �`�揉��������
	/// </summary>
	/// <param name="winWidth"> �E�B���h�E���� </param>
	/// <param name="winHeight"> �E�B���h�E�c�� </param>
	void DrawInitialize(int winWidth, int winHeight);

	/// <summary>
	/// DirectX�̖��t���[������
	/// </summary>
	void DirectXUpdate(int winWidth, int winHeight);
};