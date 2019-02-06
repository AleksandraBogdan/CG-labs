#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "Interface.h"
#include "Cylinder.h"
using namespace DirectX;

void CreateCylinder(ID3D11Device* g_pd3dDevice, ID3D11Buffer** m_pConstantBuffer, ID3D11Buffer** m_pVertexBuffer, ID3D11Buffer** m_pIndexBuffer,
    float radius, float height, XMFLOAT4 color)
{
    // Create vertex buffer
    SimpleVertex vertices[CYLINDER_VERTICES + 2];

    int centreLowPointInd = 0;
    int centreUpPointInd = 1;
    vertices[centreLowPointInd] = { XMFLOAT3(0.0, 0.0,           0.0), color }; // Lower centre point
    vertices[centreUpPointInd] = { XMFLOAT3(0.0, height, 0.0), color }; // Upper centre point
    int shift = 2;

    float angleStep = 2 * XM_2PI / (CYLINDER_VERTICES);
    float angle = 0.;
    for (int i = 0; i < CYLINDER_N; i++, angle += angleStep)
    {
        vertices[shift + 2 * i] = { XMFLOAT3(sinf(angle) * radius, 0.,            cosf(angle) * radius), color };
        vertices[shift + 2 * i + 1] = { XMFLOAT3(sinf(angle) * radius, height, cosf(angle) * radius), color };
    }

    // Create index buffer
    WORD indices[CYLINDER_INDICES];
    for (WORD i = 0; i < CYLINDER_N; i++) {
        int leftDownInd = shift + i * 2;
        int leftUpInd = shift + 1 + i * 2;
        int rightDownInd;
        int rightUpInd;
        if (shift + (i + 1) * 2 < CYLINDER_VERTICES + shift)
        {
            rightDownInd = (shift + (i + 1) * 2);
            rightUpInd = (shift + 1 + (i + 1) * 2);
        }
        else
        {
            rightDownInd = (shift + (i + 1) * 2) % CYLINDER_VERTICES;
            rightUpInd = (shift + 1 + (i + 1) * 2) % CYLINDER_VERTICES;
        }
        // Lower triangle
        indices[12 * i + 2] = WORD(leftDownInd);
        indices[12 * i + 1] = WORD(rightDownInd);
        indices[12 * i + 0] = WORD(centreLowPointInd);
        // Upper triangle
        indices[12 * i + 5] = WORD(leftUpInd);
        indices[12 * i + 4] = WORD(centreUpPointInd);
        indices[12 * i + 3] = WORD(rightUpInd);
        // Lower side triangle
        indices[12 * i + 8] = WORD(leftDownInd);
        indices[12 * i + 7] = WORD(leftUpInd);
        indices[12 * i + 6] = WORD(rightDownInd);
        // Upper side triangle
        indices[12 * i + 11] = WORD(rightDownInd);
        indices[12 * i + 10] = WORD(leftUpInd);
        indices[12 * i + 9] = WORD(rightUpInd);
    }

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * (CYLINDER_VERTICES + 2);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;
    g_pd3dDevice->CreateBuffer(&bd, &InitData, m_pVertexBuffer);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * CYLINDER_INDICES;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    g_pd3dDevice->CreateBuffer(&bd, &InitData, m_pIndexBuffer);

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    g_pd3dDevice->CreateBuffer(&bd, nullptr, m_pConstantBuffer);
}