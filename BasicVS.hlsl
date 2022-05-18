#include "Basic.hlsli"

// 3D変換行列
cbuffer ConstBufferDataTransform : register(b1)
{
	matrix mat;// -> 3D変換行列
}

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput output;// -> ピクセルシェーダーに渡す値
	output.svpos = mul(mat, pos);
	output.uv = uv;
	return output;
}