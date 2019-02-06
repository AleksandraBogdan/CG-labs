#pragma once
#ifndef CARGO_H
#define CARGO_H
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "Interface.h"

using namespace DirectX;
#define CARGO_RADIUS 0.2f
#define CARGO_HEIGHT 1.f
#define CARGO_COLOR { 1.0, 0.0, 0.0, 1.0 }

class Cargo : public Interface
{
private:
    ID3D11Buffer* m_pConstantBuffer = nullptr;
    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;

    Interface * child;
    XMFLOAT3 pos;
	XMFLOAT4 color;
public:
    Cargo(XMFLOAT3 pos, Interface* child, XMFLOAT4 color);
    void Init(ID3D11Device* g_pd3dDevice);
    void Render(ID3D11DeviceContext* g_pImmediateContext, float &t, XMMATRIX g_World, XMMATRIX & g_View, XMMATRIX & g_Projection);
    void Release();
    ~Cargo();
};

#endif