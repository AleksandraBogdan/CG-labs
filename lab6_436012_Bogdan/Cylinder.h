#define CYLINDER_N 11
#define CYLINDER_VERTICES (CYLINDER_N * 2)
#define CYLINDER_INDICES (CYLINDER_N * 4 * 3)

void CreateCylinder(ID3D11Device* g_pd3dDevice, ID3D11Buffer** m_pConstantBuffer, ID3D11Buffer** m_pVertexBuffer, ID3D11Buffer** m_pIndexBuffer,
    float radius, float height, XMFLOAT4 color);