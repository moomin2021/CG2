#include "Basic.hlsli"

// 3D�ϊ��s��
cbuffer ConstBufferDataTransform : register(b1)
{
	matrix mat;// -> 3D�ϊ��s��
}

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput output;// -> �s�N�Z���V�F�[�_�[�ɓn���l
	output.svpos = mul(mat, pos);
	output.uv = uv;
	return output;
}