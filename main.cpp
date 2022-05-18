#include "Input.h"
#include <DirectXteX.h>
#include "Window.h"
#include "DirectXManager.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>


// D3Dコンパイラのインクルード
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <DirectXMath.h>
using namespace DirectX;

// Windowsアプリでエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウのクラス生成
	Window * win = new Window();

	// ウィンドウの初期化
	win->WindowInitialize();

	// DirectX管理クラス生成
	DirectXManager * directXM = new DirectXManager();

	// DirectXの初期化
	directXM->DirectXInitialize(win->hwnd);

	// DirectX初期化処理 ここから

#pragma region
	// Inputクラス生成
	Input * input = new Input();

	// キーボード情報の初期化
	input->InputInitialize(win->w, win->hwnd);

#pragma endregion

	// DirectX初期化処理 ここまで

	// 描画初期化処理　ここから

#pragma region

	// 頂点データの構造体
	struct Vertex
	{
		XMFLOAT3 pos;// -> xyz座標
		XMFLOAT2 uv;// --> uv座標
	};

	// 頂点データ
	Vertex vertices[] =
	{
		{{  0.0f, 100.0f, 0.0f}, {0.0f, 1.0f}},// 左下
		{{  0.0f,   0.0f, 0.0f}, {0.0f, 0.0f}},// 左上
		{{100.0f, 100.0f, 0.0f}, {1.0f, 1.0f}},// 右下
		{{100.0f,   0.0f, 0.0f}, {1.0f, 0.0f}},// 右上
	};

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT result;

	// 頂点バッファの生成
	ID3D12Resource * vertBuff = nullptr;
	result = directXM->device->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex * vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);

	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	// 頂点データ1つ分のサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);

	ID3DBlob * vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob * psBlob = nullptr; // ピクセルシェーダオブジェクト
	ID3DBlob * errorBlob = nullptr; // エラーオブジェクト
	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicVS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	// エラーなら
	if (FAILED(result))
	{
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char *)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicPS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	// エラーなら
	if (FAILED(result))
	{
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char *)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			// xyz座標
			"POSITION", // ---------------------------------> セマンティック名
			0, // ------------------------------------------> 同じセマンティック名が複数あるときに使うインデックス（0でよい）
			DXGI_FORMAT_R32G32B32_FLOAT, // ----------------> 要素数とビット数を表す（XYZの三つでfloat型なのでR326G32B32_FLOAT）
			0, // ------------------------------------------> 入力スロットインデックス（0でよい）
			D3D12_APPEND_ALIGNED_ELEMENT, // ---------------> データのオフセット値（D3D12_APPEND_ALIGNED_ELEMENTだと自動設定）
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // -> 入力データ種別（標準はD3D12_INPUT_CLASSIFICATTON_PER_VERTEX_DATA）
			0 // -------------------------------------------> 一度に描写するインスタンス数（0でよい）
		},
		{
			// uv座標
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// ラスタライザの設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	// ブレンドステート
	D3D12_RENDER_TARGET_BLEND_DESC & blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA全てのチャンネルを描画

	// 共通設定
	blenddesc.BlendEnable = true;// ----------------> ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;// -> 加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;// ---> ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;// -> デストの値を0%使う

	// 加算合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;// -> 加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;// -> ソースの値を100%使う
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;// -> デストの値を100%使う

	// 頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// その他の設定
	pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;// -> 一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;// -> テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ルートパラメーターの設定
	D3D12_ROOT_PARAMETER rootParams[3] = {};

	// 定数バッファ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// --> 定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0;// ------------------> 定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;// -------------------> デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// -> 全てのシェーダから見える

	// テクスチャレジスタ0番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// -> 種類
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;// -------> デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;// --------------------> デスクリプタレンジ数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// -------------> 全てのシェーダーから見える

	// 定数バッファ1番
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// --> 種類
	rootParams[2].Descriptor.ShaderRegister = 1;// ------------------> 定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0;// -------------------> デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// -> 全てのシェーダーから見える

	// テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// ----------------> 横繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// ----------------> 縦繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// ----------------> 奥行繰り返し
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;// -> ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// ------------------> 全てのリニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;// --------------------------------> ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;// ---------------------------------------------> ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// -> ピクセルシェーダーからのみ使用可能

	// ルートシグネチャ
	ID3D12RootSignature * rootSignature;

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;// -> ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);// -> ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	// ルートシグネチャのシリアライズ
	ID3DBlob * rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = directXM->device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature;

	// パイプランステートの生成
	ID3D12PipelineState * pipelineState = nullptr;
	result = directXM->device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	// 定数バッファ用データ構造
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;// 色(RGBA)
	};

	// 定数バッファ用データ構造体(3D変換行列)
	struct ConstBufferDataTransform
	{
		XMMATRIX mat;// -> 3D変換行列
	};

	ID3D12Resource * constBuffTransform = nullptr;
	ConstBufferDataTransform * constMapTransform = nullptr;

	{
		// ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;// -> GPUへの転送用

		// リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xFF) & ~0xFF;// -> 256バイトアラインメント
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		// 定数バッファの生成
		result = directXM->device->CreateCommittedResource(
			&cbHeapProp,// -> ヒープ設定
			D3D12_HEAP_FLAG_NONE,
			&cbResourceDesc,// -> リソース設定
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuffTransform));
		assert(SUCCEEDED(result));

		result = constBuffTransform->Map(0, nullptr, (void **)&constMapTransform);// -> マッピング
		assert(SUCCEEDED(result));
	}

	// 単位行列を代入
	constMapTransform->mat = XMMatrixIdentity();

	constMapTransform->mat.r[0].m128_f32[0] = 2.0f / win->GetWindowWidth();
	constMapTransform->mat.r[1].m128_f32[1] = -2.0f / win->GetWindowHeight();

	constMapTransform->mat.r[3].m128_f32[0] = -1.0f;
	constMapTransform->mat.r[3].m128_f32[1] =  1.0f;

	// ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;// -> GPUへの転送用

	// リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xFF) & ~0xFF;// -> 256バイトアラインメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource * constBuffMaterial = nullptr;

	// 定数バッファの生成
	result = directXM->device->CreateCommittedResource(
		&cbHeapProp,// -> ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,// -> リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	// 定数バッファのマッピング
	ConstBufferDataMaterial * constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void **)&constMapMaterial);// -> マッピング

	// 値を書き込むと自動的に転送される
	constMapMaterial->color = XMFLOAT4(1, 0, 0, 0.5f);// -> RGBAで半透明の赤

	// インデックスデータ
	unsigned short indices[] =
	{
		0, 1, 2,// 三角形1つ目
		1, 2, 3,// 三角形2つ目
	};

	// インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	// リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;// -> インデックス情報が入るサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// インデックスバッファの生成
	ID3D12Resource * indexBuff = nullptr;
	result = directXM->device->CreateCommittedResource(
		&heapProp,// -> ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,// -> リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	// インデックスバッファをマッピング
	uint16_t * indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
	// 全インデックスに対して
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];// -> インデックスをコピー
	}

	// マッピングを解除
	indexBuff->Unmap(0, nullptr);

	// インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// WICテクスチャロード
	result = LoadFromWICFile(
		L"Resources/texture.jpg",
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	ScratchImage mipChain{};

	// ミニマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain
	);

	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// 読み込んだディフェーズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;// ---> 幅
	textureResourceDesc.Height = (UINT)metadata.height;// -> 高さ
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	// テクスチャバッファの生成
	ID3D12Resource * texBuff = nullptr;
	result = directXM->device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);

	// 全ミップマップについて
	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		// ミップマップレベルを指定してイメージを取得
		const Image * img = scratchImg.GetImage(i, 0, 0);

		// テクスチャバッファにデータ転送
		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,// --------------> 全領域へコピー
			img->pixels,// ----------> 元データアドレス
			(UINT)img->rowPitch,// --> 1ラインサイズ
			(UINT)img->slicePitch// -> 1枚サイズ
		);
		assert(SUCCEEDED(result));
	}

	// SRVの最大個数
	const size_t kMaxSRVCount = 2056;

	// デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	// 設定を元にSRV用デスクリプタヒープを生成
	ID3D12DescriptorHeap * srvHeap = nullptr;
	result = directXM->device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	// SRVヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	// シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};// -> 設定構造体
	srvDesc.Format = resDesc.Format;// -> RGBA float
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;// -> 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	// ハンドルの指す位置にシェーダーリソースビュー作成
	directXM->device->CreateShaderResourceView(texBuff, &srvDesc, srvHandle);

#pragma endregion

	// 描画初期化処理　ここまで

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

		// キーボードで2が押されたら図形を塗りつぶしかワイヤーフレームにするか切り替える
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

		// バックバッファの番号を取得(2つなので0番か1番)
		UINT bbIndex = directXM->swapChain->GetCurrentBackBufferIndex();

		// 1.リソースバリアで書き込み可能に変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = directXM->backBuffers[bbIndex]; // バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
		directXM->commandList->ResourceBarrier(1, &barrierDesc);

		// 2.描画先の変更

		// レンダーターゲットビューのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = directXM->rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * directXM->device->GetDescriptorHandleIncrementSize(directXM->rtvHeapDesc.Type);
		directXM->commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		// 3.画面クリア R G B A
		FLOAT clearColor[] = { 0.1f ,0.25f, 0.5f, 0.0f }; // 青っぽい色

		// スペースキーを押されている間、色を変える
		if (input->KeyDown(DIK_SPACE))
		{
			clearColor[0] = 1.0f;
			clearColor[1] = 0.0f;
			clearColor[2] = 0.6f;
			clearColor[3] = 0.0f;
		}

		directXM->commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		// 4.描画コマンドここから

#pragma region

// ビューポート設定コマンド

		// シザー矩形
		D3D12_RECT scissorRect{};
		scissorRect.left = 0; // 切り抜き座標左
		scissorRect.right = scissorRect.left + win->GetWindowWidth(); // 切り抜き座標右
		scissorRect.top = 0; // 切り抜き座標上
		scissorRect.bottom = scissorRect.top + win->GetWindowHeight(); // 切り抜き座標下
		// シザー矩形設定コマンドを、コマンドリストに積む
		directXM->commandList->RSSetScissorRects(1, &scissorRect);

		// パイプラインステートとルートシグネチャの設定コマンド
		directXM->commandList->SetPipelineState(pipelineState);
		directXM->commandList->SetGraphicsRootSignature(rootSignature);

		// プリミティブ形状の設定コマンド
		directXM->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形リスト

		// 頂点バッファビューの設定コマンド
		directXM->commandList->IASetVertexBuffers(0, 1, &vbView);

		// インデックスバッファビューの設定コマンド
		directXM->commandList->IASetIndexBuffer(&ibView);

		// -左上のビューポート設定- //
		D3D12_VIEWPORT viewport{};
		viewport.Width = win->GetWindowWidth();
		viewport.Height = win->GetWindowHeight();
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		// ビューポート設定コマンドを、コマンドリストに積む
		directXM->commandList->RSSetViewports(1, &viewport);

		// 定数バッファビュー(CBV)の設定コマンド
		directXM->commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		// SRVヒープの設定コマンド
		directXM->commandList->SetDescriptorHeaps(1, &srvHeap);

		// SRVヒープの先頭ハンドルを取得(SRVを指しているはず)
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();

		// SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
		directXM->commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

		// 定数バッファビュー(CBV)の設定コマンド
		directXM->commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

		// 描画コマンド
		directXM->commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // 全ての頂点を使って描画

#pragma endregion

		// 4.描画コマンドここまで

		// 5.リソースバリアを戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 表示状態へ
		directXM->commandList->ResourceBarrier(1, &barrierDesc);

		// 命令のクローズ
		result = directXM->commandList->Close();
		assert(SUCCEEDED(result));
		// コマンドリストの実行
		ID3D12CommandList * commandLists[] = { directXM->commandList };
		directXM->commandQueue->ExecuteCommandLists(1, commandLists);
		// 画面に表示するバッファをフリップ(裏表の入替え)
		result = directXM->swapChain->Present(1, 0);
		assert(SUCCEEDED(result));

		// コマンドの実行完了を待つ
		directXM->commandQueue->Signal(directXM->fence, ++directXM->fenceVal);
		if (directXM->fence->GetCompletedValue() != directXM->fenceVal)
		{
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			directXM->fence->SetEventOnCompletion(directXM->fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		// キューをクリア
		result = directXM->cmdAllocator->Reset();
		assert(SUCCEEDED(result));
		// 再びコマンドリストを貯める準備
		result = directXM->commandList->Reset(directXM->cmdAllocator, nullptr);
		assert(SUCCEEDED(result));

#pragma endregion

		// DirectX毎フレーム処理　ここまで
	}

	return 0;
}