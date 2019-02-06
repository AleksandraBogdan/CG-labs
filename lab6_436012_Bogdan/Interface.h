#pragma once
#ifndef INTERFACE_H
#define INTERFACE_H
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <windows.h>
#include <vector>
#include <stdlib.h>

using namespace DirectX;

struct SimpleVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

struct ConstantBuffer
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
};

class Interface {
public:
	virtual void Init(ID3D11Device* g_pd3dDevice) = 0;
	virtual void Render(ID3D11DeviceContext* g_pImmediateContext, float &t, XMMATRIX g_World, XMMATRIX & g_View, XMMATRIX & g_Projection) = 0;
	virtual void Release() = 0;
};

#endif