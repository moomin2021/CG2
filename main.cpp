#include "Input.h"
#include <DirectXteX.h>
#include "Window.h"
#include "DirectXManager.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>


// D3D�R���p�C���̃C���N���[�h
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <DirectXMath.h>
using namespace DirectX;

// Windows�A�v���ŃG���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// �E�B���h�E�̃N���X����
	Window * win = new Window();

	// �E�B���h�E�̏�����
	win->WindowInitialize();

	// DirectX�Ǘ��N���X����
	DirectXManager * directXM = new DirectXManager();

	// DirectX�̏�����
	directXM->DirectXInitialize(win->hwnd);

	// DirectX���������� ��������

#pragma region
	// Input�N���X����
	Input * input = new Input();

	// �L�[�{�[�h���̏�����
	input->InputInitialize(win->w, win->hwnd);

#pragma endregion

	// DirectX���������� �����܂�

	// �`�揉���������@��������

#pragma region

	// ���_�f�[�^�̍\����
	struct Vertex
	{
		XMFLOAT3 pos;// -> xyz���W
		XMFLOAT2 uv;// --> uv���W
	};

	// ���_�f�[�^
	Vertex vertices[] =
	{
		{{  0.0f, 100.0f, 0.0f}, {0.0f, 1.0f}},// ����
		{{  0.0f,   0.0f, 0.0f}, {0.0f, 0.0f}},// ����
		{{100.0f, 100.0f, 0.0f}, {1.0f, 1.0f}},// �E��
		{{100.0f,   0.0f, 0.0f}, {1.0f, 0.0f}},// �E��
	};

	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	// ���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{}; // �q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // ���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT result;

	// ���_�o�b�t�@�̐���
	ID3D12Resource * vertBuff = nullptr;
	result = directXM->device->CreateCommittedResource(
		&heapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	Vertex * vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
	assert(SUCCEEDED(result));
	// �S���_�ɑ΂���
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i]; // ���W���R�s�[
	}
	// �q���������
	vertBuff->Unmap(0, nullptr);

	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	// ���_�f�[�^1���̃T�C�Y
	vbView.StrideInBytes = sizeof(vertices[0]);

	ID3DBlob * vsBlob = nullptr; // ���_�V�F�[�_�I�u�W�F�N�g
	ID3DBlob * psBlob = nullptr; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ID3DBlob * errorBlob = nullptr; // �G���[�I�u�W�F�N�g
	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"BasicVS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);

	// �G���[�Ȃ�
	if (FAILED(result))
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char *)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"BasicPS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);

	// �G���[�Ȃ�
	if (FAILED(result))
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char *)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			// xyz���W
			"POSITION", // ---------------------------------> �Z�}���e�B�b�N��
			0, // ------------------------------------------> �����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X�i0�ł悢�j
			DXGI_FORMAT_R32G32B32_FLOAT, // ----------------> �v�f���ƃr�b�g����\���iXYZ�̎O��float�^�Ȃ̂�R326G32B32_FLOAT�j
			0, // ------------------------------------------> ���̓X���b�g�C���f�b�N�X�i0�ł悢�j
			D3D12_APPEND_ALIGNED_ELEMENT, // ---------------> �f�[�^�̃I�t�Z�b�g�l�iD3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�j
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // -> ���̓f�[�^��ʁi�W����D3D12_INPUT_CLASSIFICATTON_PER_VERTEX_DATA�j
			0 // -------------------------------------------> ��x�ɕ`�ʂ���C���X�^���X���i0�ł悢�j
		},
		{
			// uv���W
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// �V�F�[�_�[�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	// ���X�^���C�U�̐ݒ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O���Ȃ�
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�
	pipelineDesc.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

	// �u�����h�X�e�[�g
	D3D12_RENDER_TARGET_BLEND_DESC & blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA�S�Ẵ`�����l����`��

	// ���ʐݒ�
	blenddesc.BlendEnable = true;// ----------------> �u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;// -> ���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;// ---> �\�[�X�̒l��100%�g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;// -> �f�X�g�̒l��0%�g��

	// ���Z����
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;// -> ���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;// -> �\�[�X�̒l��100%�g��
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;// -> �f�X�g�̒l��100%�g��

	// ���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1; // �`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;// -> ��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;// -> �e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ���[�g�p�����[�^�[�̐ݒ�
	D3D12_ROOT_PARAMETER rootParams[3] = {};

	// �萔�o�b�t�@0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// --> �萔�o�b�t�@�r���[
	rootParams[0].Descriptor.ShaderRegister = 0;// ------------------> �萔�o�b�t�@�ԍ�
	rootParams[0].Descriptor.RegisterSpace = 0;// -------------------> �f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// -> �S�ẴV�F�[�_���猩����

	// �e�N�X�`�����W�X�^0��
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// -> ���
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;// -------> �f�X�N���v�^�����W
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;// --------------------> �f�X�N���v�^�����W��
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// -------------> �S�ẴV�F�[�_�[���猩����

	// �萔�o�b�t�@1��
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// --> ���
	rootParams[2].Descriptor.ShaderRegister = 1;// ------------------> �萔�o�b�t�@�ԍ�
	rootParams[2].Descriptor.RegisterSpace = 0;// -------------------> �f�t�H���g�l
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// -> �S�ẴV�F�[�_�[���猩����

	// �e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// ----------------> ���J��Ԃ�
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// ----------------> �c�J��Ԃ�
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// ----------------> ���s�J��Ԃ�
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;// -> �{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// ------------------> �S�Ẵ��j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;// --------------------------------> �~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;// ---------------------------------------------> �~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// -> �s�N�Z���V�F�[�_�[����̂ݎg�p�\

	// ���[�g�V�O�l�`��
	ID3D12RootSignature * rootSignature;

	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;// -> ���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams);// -> ���[�g�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	// ���[�g�V�O�l�`���̃V���A���C�Y
	ID3DBlob * rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = directXM->device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature;

	// �p�C�v�����X�e�[�g�̐���
	ID3D12PipelineState * pipelineState = nullptr;
	result = directXM->device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	// �萔�o�b�t�@�p�f�[�^�\��
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;// �F(RGBA)
	};

	// �萔�o�b�t�@�p�f�[�^�\����(3D�ϊ��s��)
	struct ConstBufferDataTransform
	{
		XMMATRIX mat;// -> 3D�ϊ��s��
	};

	ID3D12Resource * constBuffTransform = nullptr;
	ConstBufferDataTransform * constMapTransform = nullptr;

	{
		// �q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;// -> GPU�ւ̓]���p

		// ���\�[�X�ݒ�
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xFF) & ~0xFF;// -> 256�o�C�g�A���C�������g
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		// �萔�o�b�t�@�̐���
		result = directXM->device->CreateCommittedResource(
			&cbHeapProp,// -> �q�[�v�ݒ�
			D3D12_HEAP_FLAG_NONE,
			&cbResourceDesc,// -> ���\�[�X�ݒ�
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuffTransform));
		assert(SUCCEEDED(result));

		result = constBuffTransform->Map(0, nullptr, (void **)&constMapTransform);// -> �}�b�s���O
		assert(SUCCEEDED(result));
	}

	// �P�ʍs�����
	constMapTransform->mat = XMMatrixIdentity();

	constMapTransform->mat.r[0].m128_f32[0] = 2.0f / win->GetWindowWidth();
	constMapTransform->mat.r[1].m128_f32[1] = -2.0f / win->GetWindowHeight();

	constMapTransform->mat.r[3].m128_f32[0] = -1.0f;
	constMapTransform->mat.r[3].m128_f32[1] =  1.0f;

	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;// -> GPU�ւ̓]���p

	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xFF) & ~0xFF;// -> 256�o�C�g�A���C�������g
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource * constBuffMaterial = nullptr;

	// �萔�o�b�t�@�̐���
	result = directXM->device->CreateCommittedResource(
		&cbHeapProp,// -> �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,// -> ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	// �萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial * constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void **)&constMapMaterial);// -> �}�b�s���O

	// �l���������ނƎ����I�ɓ]�������
	constMapMaterial->color = XMFLOAT4(1, 0, 0, 0.5f);// -> RGBA�Ŕ������̐�

	// �C���f�b�N�X�f�[�^
	unsigned short indices[] =
	{
		0, 1, 2,// �O�p�`1��
		1, 2, 3,// �O�p�`2��
	};

	// �C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	// ���\�[�X�ݒ�
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;// -> �C���f�b�N�X��񂪓���T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// �C���f�b�N�X�o�b�t�@�̐���
	ID3D12Resource * indexBuff = nullptr;
	result = directXM->device->CreateCommittedResource(
		&heapProp,// -> �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc,// -> ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	// �C���f�b�N�X�o�b�t�@���}�b�s���O
	uint16_t * indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
	// �S�C���f�b�N�X�ɑ΂���
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];// -> �C���f�b�N�X���R�s�[
	}

	// �}�b�s���O������
	indexBuff->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// WIC�e�N�X�`�����[�h
	result = LoadFromWICFile(
		L"Resources/texture.jpg",
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	ScratchImage mipChain{};

	// �~�j�}�b�v����
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain
	);

	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// �ǂݍ��񂾃f�B�t�F�[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);

	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;// ---> ��
	textureResourceDesc.Height = (UINT)metadata.height;// -> ����
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	// �e�N�X�`���o�b�t�@�̐���
	ID3D12Resource * texBuff = nullptr;
	result = directXM->device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);

	// �S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		// �~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image * img = scratchImg.GetImage(i, 0, 0);

		// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,// --------------> �S�̈�փR�s�[
			img->pixels,// ----------> ���f�[�^�A�h���X
			(UINT)img->rowPitch,// --> 1���C���T�C�Y
			(UINT)img->slicePitch// -> 1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}

	// SRV�̍ő��
	const size_t kMaxSRVCount = 2056;

	// �f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	// �ݒ������SRV�p�f�X�N���v�^�q�[�v�𐶐�
	ID3D12DescriptorHeap * srvHeap = nullptr;
	result = directXM->device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	// SRV�q�[�v�̐擪�n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	// �V�F�[�_�[���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};// -> �ݒ�\����
	srvDesc.Format = resDesc.Format;// -> RGBA float
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;// -> 2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	// �n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	directXM->device->CreateShaderResourceView(texBuff, &srvDesc, srvHandle);

#pragma endregion

	// �`�揉���������@�����܂�

	// �Q�[�����[�v
	while (true)
	{
		// ���b�Z�[�W������H
		if (PeekMessage(&win->msg, nullptr, 0, 0, PM_REMOVE))
		{
			// �L�[���̓��b�Z�[�W����
			TranslateMessage(&win->msg);

			// �v���V�[�W���Ƀ��b�Z�[�W�𑗂�
			DispatchMessage(&win->msg);
		}

		// �~�{�^���ŏI�����b�Z�[�W��������Q�[�����[�v�𔲂���
		if (win->msg.message == WM_QUIT)
		{
			delete win;
			break;
		}

		// DirectX���t���[�������@��������

#pragma region

		// �L�[�{�[�h�̍X�V����
		input->InputUpdate();

		// �L�[�{�[�h��2�������ꂽ��}�`��h��Ԃ������C���[�t���[���ɂ��邩�؂�ւ���
		if (input->KeyTrigger(DIK_2))
		{
			if (pipelineDesc.RasterizerState.FillMode == D3D12_FILL_MODE_SOLID)
			{
				pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
			}
			else
			{
				pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
			}
			result = directXM->device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
			assert(SUCCEEDED(result));
		}

		// �o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
		UINT bbIndex = directXM->swapChain->GetCurrentBackBufferIndex();

		// 1.���\�[�X�o���A�ŏ������݉\�ɕύX
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = directXM->backBuffers[bbIndex]; // �o�b�N�o�b�t�@���w��
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ�
		directXM->commandList->ResourceBarrier(1, &barrierDesc);

		// 2.�`���̕ύX

		// �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = directXM->rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * directXM->device->GetDescriptorHandleIncrementSize(directXM->rtvHeapDesc.Type);
		directXM->commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		// 3.��ʃN���A R G B A
		FLOAT clearColor[] = { 0.1f ,0.25f, 0.5f, 0.0f }; // ���ۂ��F

		// �X�y�[�X�L�[��������Ă���ԁA�F��ς���
		if (input->KeyDown(DIK_SPACE))
		{
			clearColor[0] = 1.0f;
			clearColor[1] = 0.0f;
			clearColor[2] = 0.6f;
			clearColor[3] = 0.0f;
		}

		directXM->commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		// 4.�`��R�}���h��������

#pragma region

// �r���[�|�[�g�ݒ�R�}���h

		// �V�U�[��`
		D3D12_RECT scissorRect{};
		scissorRect.left = 0; // �؂蔲�����W��
		scissorRect.right = scissorRect.left + win->GetWindowWidth(); // �؂蔲�����W�E
		scissorRect.top = 0; // �؂蔲�����W��
		scissorRect.bottom = scissorRect.top + win->GetWindowHeight(); // �؂蔲�����W��
		// �V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		directXM->commandList->RSSetScissorRects(1, &scissorRect);

		// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
		directXM->commandList->SetPipelineState(pipelineState);
		directXM->commandList->SetGraphicsRootSignature(rootSignature);

		// �v���~�e�B�u�`��̐ݒ�R�}���h
		directXM->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`���X�g

		// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
		directXM->commandList->IASetVertexBuffers(0, 1, &vbView);

		// �C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
		directXM->commandList->IASetIndexBuffer(&ibView);

		// -����̃r���[�|�[�g�ݒ�- //
		D3D12_VIEWPORT viewport{};
		viewport.Width = win->GetWindowWidth();
		viewport.Height = win->GetWindowHeight();
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		// �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		directXM->commandList->RSSetViewports(1, &viewport);

		// �萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
		directXM->commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		// SRV�q�[�v�̐ݒ�R�}���h
		directXM->commandList->SetDescriptorHeaps(1, &srvHeap);

		// SRV�q�[�v�̐擪�n���h�����擾(SRV���w���Ă���͂�)
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();

		// SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�Ԃɐݒ�
		directXM->commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

		// �萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
		directXM->commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

		// �`��R�}���h
		directXM->commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��

#pragma endregion

		// 4.�`��R�}���h�����܂�

		// 5.���\�[�X�o���A��߂�
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ�
		directXM->commandList->ResourceBarrier(1, &barrierDesc);

		// ���߂̃N���[�Y
		result = directXM->commandList->Close();
		assert(SUCCEEDED(result));
		// �R�}���h���X�g�̎��s
		ID3D12CommandList * commandLists[] = { directXM->commandList };
		directXM->commandQueue->ExecuteCommandLists(1, commandLists);
		// ��ʂɕ\������o�b�t�@���t���b�v(���\�̓��ւ�)
		result = directXM->swapChain->Present(1, 0);
		assert(SUCCEEDED(result));

		// �R�}���h�̎��s������҂�
		directXM->commandQueue->Signal(directXM->fence, ++directXM->fenceVal);
		if (directXM->fence->GetCompletedValue() != directXM->fenceVal)
		{
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			directXM->fence->SetEventOnCompletion(directXM->fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		// �L���[���N���A
		result = directXM->cmdAllocator->Reset();
		assert(SUCCEEDED(result));
		// �ĂуR�}���h���X�g�𒙂߂鏀��
		result = directXM->commandList->Reset(directXM->cmdAllocator, nullptr);
		assert(SUCCEEDED(result));

#pragma endregion

		// DirectX���t���[�������@�����܂�
	}

	return 0;
}