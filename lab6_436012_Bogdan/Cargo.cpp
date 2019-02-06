#include "Cargo.h"
#include "Cylinder.h"

Cargo::Cargo(XMFLOAT3 pos, Interface* child, XMFLOAT4 color)
{
    this->child = child;
    this->pos = pos;
	this->color = color;

}

void Cargo::Init(ID3D11Device* g_pd3dDevice)
{
    // Create Cylinder
    CreateCylinder(g_pd3dDevice, &m_pConstantBuffer, &m_pVertexBuffer, &m_pIndexBuffer, CARGO_RADIUS, CARGO_HEIGHT, this->color);
}


void Cargo::Render(ID3D11DeviceContext* g_pImmediateContext, float &t, XMMATRIX g_World, XMMATRIX & g_View, XMMATRIX & g_Projection)
{
    // Move thread
    XMMATRIX move = XMMatrixTranslation(pos.x, pos.y, pos.z);
    g_World = move * g_World;

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
    g_pImmediateContext->DrawIndexed(CYLINDER_INDICES, 0, 0);
    if (child) child->Render(g_pImmediateContext, t, g_World, g_View, g_Projection); //изменить матрицы
}

void Cargo::Release()
{
    if (m_pConstantBuffer) m_pConstantBuffer->Release();
    if (m_pVertexBuffer) m_pVertexBuffer->Release();
    if (m_pIndexBuffer) m_pIndexBuffer->Release();

    if (child) child->Release();
}


Cargo::~Cargo()
{
    Release();
}
