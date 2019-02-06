#include "WindMusic.h"
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "StartFigure.h"
#include "Interface.h"
#include "Cross.h"
#include "Cargo.h"

using namespace DirectX;

#define START_FUGURE_POS {0.0, 3.0, 0.0}
#define THREAD1_POS {0.0, -THREAD_HEIGHT, 0.0}
#define CROSS_POS {0.0, 0.0, 0.0}
#define THREAD2_POS_Y -THREAD_HEIGHT
#define CARGO1_POS {0.0, -CARGO_HEIGHT, 0.0}
#define THREAD3_POS_Y -THREAD_HEIGHT
#define CARGO2_POS {0.0, -CARGO_HEIGHT, 0.0}

WindMusic::WindMusic(XMFLOAT3 pos)
{
    this->pos = pos;
}
void WindMusic::Init(ID3D11Device* g_pd3dDevice)
{
    float angleStep = XM_2PI / CROSS_CHILDS_NUM;
    float angle = 0.0;
    Interface* threads2345[4];
	XMFLOAT4 colors[8] = { XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) ,XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) , XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) ,
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) ,XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) ,XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) ,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) ,XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	int count = 0;

    for (int i = 0; i < CROSS_CHILDS_NUM; i++, angle += angleStep)
    {

        Interface* child = NULL;
        if (i == 0 || i == 3)
        {
            Interface *cargo2 = new Cargo(CARGO2_POS, NULL, colors[count]);
            cargo2->Init(g_pd3dDevice);
			++count;

            XMFLOAT3 thread3Pos = { 0.0, THREAD3_POS_Y, 0.0 };
            Interface *thread3 = new Thread(thread3Pos, cargo2);
            thread3->Init(g_pd3dDevice);
            child = thread3;
        }

        Interface *cargo1 = new Cargo(CARGO1_POS, child, colors[count]);
        cargo1->Init(g_pd3dDevice);
		++count;

        float x = float((CROSS_LENGTH / 2 - 0.1) * sin(angle));
        float z = float((CROSS_LENGTH / 2 - 0.1) * cos(angle));
        XMFLOAT3 thread2Pos = { x, THREAD2_POS_Y, z };
        Interface* thread2 = new Thread(thread2Pos, cargo1);
        thread2->Init(g_pd3dDevice);
        threads2345[i] = thread2;
    }
    // Build from the bottom up
    Interface* cross = new Cross(CROSS_POS, threads2345);
    cross->Init(g_pd3dDevice);

    Interface *thread1 = new Thread(THREAD1_POS, cross);
    thread1->Init(g_pd3dDevice);

    startFigure = new StartFigure(this->pos, thread1);
    startFigure->Init(g_pd3dDevice);

}
void WindMusic::Render(ID3D11DeviceContext* g_pImmediateContext, float &t, XMMATRIX g_World, XMMATRIX & g_View, XMMATRIX & g_Projection)
{
    startFigure->Render(g_pImmediateContext, t, g_World, g_View, g_Projection);
}

void WindMusic::Release()
{
    startFigure->Release();
}
WindMusic::~WindMusic()
{
    Release();
}