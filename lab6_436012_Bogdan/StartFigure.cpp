#include "StartFigure.h"

StartFigure::StartFigure(XMFLOAT3 pos, Interface* child)
{
	this->child = child;
    this->pos = pos;
}
void StartFigure::Init(ID3D11Device* g_pd3dDevice)
{
    // Create Sphere

	// Create vertex buffer
	SimpleVertex vertices[START_FIGURE_N * START_FIGURE_N];
	float t = 0, f = 0, r = 0.25f;
	for (int i = 0; i < START_FIGURE_N; i++) {
		for (int j = 0; j < START_FIGURE_N; j++) {
			float x = r * sinf(t) * cosf(f);
			float y = r * sinf(t) * sinf(f);
			float z = r * cosf(t);
			vertices[j + START_FIGURE_N * i] = { XMFLOAT3(x, y , z), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) };
			f += XM_2PI / (START_FIGURE_N - 2);
		}
		t += XM_2PI / (START_FIGURE_N - 2);
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * START_FIGURE_N*START_FIGURE_N;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	g_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);

	WORD indices[START_FIGURE_NUM_TR * 3];
	int k = 0, i = 0, j = 0;
	for (i = 0; i < START_FIGURE_N - 1; i++) {
		for (j = 0; j < START_FIGURE_N - 1; j++) {
			indices[k++] = (WORD)(j + i * START_FIGURE_N);
			indices[k++] = (WORD)(j + 1 + i * START_FIGURE_N);
			indices[k++] = (WORD)(j + 1 + START_FIGURE_N + i * START_FIGURE_N);
		}
		indices[k++] = (WORD)(j + i * START_FIGURE_N);
		indices[k++] = (WORD)(i * START_FIGURE_N);
		indices[k++] = (WORD)(i * START_FIGURE_N + START_FIGURE_N);
	}

	i = 1, j = 0;
	for (i = 1; i < START_FIGURE_N; i++) {
		for (j = 0; j < START_FIGURE_N - 1; j++) {
			indices[k++] = (WORD)(j + i * START_FIGURE_N);
			indices[k++] = (WORD)(i * START_FIGURE_N - START_FIGURE_N + j);
			indices[k++] = (WORD)(j + 1 + i * START_FIGURE_N);

		}
		indices[k++] = (WORD)(j + i * START_FIGURE_N);
		indices[k++] = (WORD)(i * START_FIGURE_N - 1);
		indices[k++] = (WORD)(i * START_FIGURE_N);
	}


	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * START_FIGURE_N * START_FIGURE_N * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	g_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);

	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	g_pd3dDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
}


void StartFigure::Render(ID3D11DeviceContext* g_pImmediateContext, float &t, XMMATRIX  g_World, XMMATRIX & g_View, XMMATRIX & g_Projection)
{
    // Move thread up
    XMMATRIX move = XMMatrixTranslation(pos.x, pos.y, pos.z);
    g_World *= move;

	//
	// Update variables
	//
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(g_World);
	cb1.mView = XMMatrixTranspose(g_View);
	cb1.mProjection = XMMatrixTranspose(g_Projection);
	g_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb1, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	g_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	g_pImmediateContext->DrawIndexed(START_FIGURE_NUM_TR * 3, 0, 0);

	//XMMATRIX mScale = XMMatrixScaling(5, 0.000001f, 5);
	//XMMATRIX newWorldMatrix = (mScale * XMMatrixTranslation(0.f, 0.75f, 0.f) * g_World);
    //newWorldMatrix = g_World;
	child->Render(g_pImmediateContext, t, g_World, g_View, g_Projection);
}

void StartFigure::Release()
{
    if(m_pConstantBuffer) m_pConstantBuffer->Release();
    if(m_pVertexBuffer) m_pVertexBuffer->Release();
    if(m_pIndexBuffer) m_pIndexBuffer->Release();

    child->Release();
}


StartFigure::~StartFigure()
{
	Release();
}
