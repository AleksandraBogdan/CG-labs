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
//#define N 20
#define CROSS_CHILDS_NUM 4
#define CROSS_RADIUS 0.1f
#define CROSS_LENGTH 3.0f
#define CROSS_COLOR {1.0, 0.8, 0.0, 0.0} // yellow

class Cross : public Interface
{
private:
	ID3D11Buffer * m_pConstantBuffer = nullptr;
	ID3D11Buffer*  m_pVertexBuffer = nullptr;
	ID3D11Buffer*  m_pIndexBuffer = nullptr;

	Interface* childs[CROSS_CHILDS_NUM];
    XMFLOAT3 pos;
public:
	Cross(XMFLOAT3 pos, Interface* childs[]);
	void Init(ID3D11Device* g_pd3dDevice);
	void Render(ID3D11DeviceContext* g_pImmediateContext, float &t, XMMATRIX g_World, XMMATRIX & g_View, XMMATRIX & g_Projection);
	void Release();
	~Cross();
};