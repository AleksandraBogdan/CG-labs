#include "Cross.h"
#include "Cylinder.h"

Cross::Cross(XMFLOAT3 pos, Interface* childs[])
{
    for (int i = 0; i < CROSS_CHILDS_NUM; i++)
    {
        this->childs[i] = childs[i];
    }
    this->pos = pos;
}

void Cross::Init(ID3D11Device* g_pd3dDevice)
{
    CreateCylinder(g_pd3dDevice, &m_pConstantBuffer, &m_pVertexBuffer, &m_pIndexBuffer, CROSS_RADIUS, CROSS_LENGTH, CROSS_COLOR);
}


void Cross::Render(ID3D11DeviceContext* g_pImmediateContext, float &t, XMMATRIX g_World, XMMATRIX & g_View, XMMATRIX & g_Projection)
{
    XMMATRIX world;
    // Rotate 
    XMMATRIX move1 = XMMatrixTranslation(0.0, -CROSS_LENGTH / 2, 0.0);
    XMMATRIX rotateZ = XMMatrixRotationZ(XM_PI / 2);
    world = move1 * rotateZ * g_World;

    // Move thread
    XMMATRIX move = XMMatrixTranslation(pos.x, pos.y, pos.z);
    world *= move;

    XMMATRIX rotateY = XMMatrixRotationY(XM_PI / 2); // Second part of cross is the same figure but rotated by 90 degree
    XMMATRIX world2 = move1 * rotateZ * rotateY * g_World;

    //
    // Update variables
    //
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;

    g_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
    g_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    ConstantBuffer cb1;
    cb1.mView = XMMatrixTranspose(g_View);
    cb1.mProjection = XMMatrixTranspose(g_Projection);

    // Draw first cylinder
    cb1.mWorld = XMMatrixTranspose(world);
    g_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb1, 0, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    g_pImmediateContext->DrawIndexed(CYLINDER_INDICES, 0, 0);

    // Draw second cylinder
    cb1.mWorld = XMMatrixTranspose(world2);
    g_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb1, 0, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    g_pImmediateContext->DrawIndexed(CYLINDER_INDICES, 0, 0);

    for (int i = 0; childs && i < CROSS_CHILDS_NUM; i++)
    {
        if(childs[i])
            childs[i]->Render(g_pImmediateContext, t, g_World, g_View, g_Projection);
    }
}

void Cross::Release()
{
    if (m_pConstantBuffer) m_pConstantBuffer->Release();
    if (m_pVertexBuffer) m_pVertexBuffer->Release();
    if (m_pIndexBuffer) m_pIndexBuffer->Release();


    for (int i = 0; childs && i < CROSS_CHILDS_NUM; i++)
    {
        if (childs[i])
            childs[i]->Release();
    }
}


Cross::~Cross()
{
	Release();
}
