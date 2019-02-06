#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "Interface.h"
#include "Thread.h"

using namespace DirectX;
#define START_FIGURE_N 20
#define START_FIGURE_NUM_TR START_FIGURE_N*2*(START_FIGURE_N-1)

class StartFigure : public Interface
{
private:
	ID3D11Buffer* m_pConstantBuffer = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;

	Interface* child;
    XMFLOAT3 pos;

public:
	StartFigure(XMFLOAT3 pos, Interface* child);
	void Init(ID3D11Device* g_pd3dDevice);
	void Render(ID3D11DeviceContext* g_pImmediateContext, float &t, XMMATRIX g_World, XMMATRIX & g_View, XMMATRIX & g_Projection);
	void Release();
	~StartFigure();
};