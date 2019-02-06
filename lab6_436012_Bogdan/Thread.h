#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "Interface.h"
#include <random>
#include <time.h>

using namespace DirectX;
#define THREAD_RADIUS 0.05f
#define THREAD_HEIGHT 0.7f
#define THREAD_COLOR { 0.0, 0.0, 0.0, 1.0 }

class Thread : public Interface
{
private:
	ID3D11Buffer* m_pConstantBuffer = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;

	Interface * child;
	XMFLOAT3 pos;
public:
	Thread(XMFLOAT3 pos, Interface* child);
	void Init(ID3D11Device* g_pd3dDevice);
	void Render(ID3D11DeviceContext* g_pImmediateContext, float &t, XMMATRIX g_World, XMMATRIX & g_View, XMMATRIX & g_Projection);
	void Release();
	~Thread();
};