#include "Thread.h"
#include "Cylinder.h"
#define THREADS_SWING_MAX_ANGLE 0.05

Thread::Thread(XMFLOAT3 pos, Interface* child)
{
    this->child = child;
    this->pos = pos;
}

void Thread::Init(ID3D11Device* g_pd3dDevice)
{
    // Create Cylinder
    CreateCylinder(g_pd3dDevice, &m_pConstantBuffer, &m_pVertexBuffer, &m_pIndexBuffer, THREAD_RADIUS, THREAD_HEIGHT, THREAD_COLOR);
}

#define DegToRadian(degree) ((degree) * (XM_PI / 180.0f))

void Thread::Render(ID3D11DeviceContext* g_pImmediateContext, float &t, XMMATRIX g_World, XMMATRIX & g_View, XMMATRIX & g_Projection)
{

    // Swing with time
    float angle = sin(t) / 2;
    XMMATRIX moveForRotate1 = XMMatrixTranslation(0.0, -THREAD_HEIGHT, 0.0);
    XMMATRIX moveForRotate2 = XMMatrixTranslation(0.0, THREAD_HEIGHT, 0.0);
    XMMATRIX rotateY = moveForRotate1 * XMMatrixRotationZ(angle) * moveForRotate2;

    // Move thread
    XMMATRIX move = XMMatrixTranslation(pos.x, pos.y, pos.z);
    g_World = rotateY * move * g_World;

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
    if (child) child->Render(g_pImmediateContext, t, g_World, g_View, g_Projection); 
}

void Thread::Release()
{
    if (m_pConstantBuffer) m_pConstantBuffer->Release();
    if (m_pVertexBuffer) m_pVertexBuffer->Release();
    if (m_pIndexBuffer) m_pIndexBuffer->Release();

    if(child) child->Release();
}


Thread::~Thread()
{
	Release();
}
