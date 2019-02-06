#pragma once
#ifndef WIND_MUSIC_FIGURE_H
#define WIND_MUSIC_FIGURE_H
#include "Interface.h"

#define WIND_MUSIC_POS 0.0, -2.0, 0.0

class WindMusic : Interface
{
    XMFLOAT3 pos;
    Interface* startFigure = nullptr;
public:
    WindMusic(XMFLOAT3 pos);
    void Init(ID3D11Device* g_pd3dDevice);
    void Render(ID3D11DeviceContext* g_pImmediateContext, float &t, XMMATRIX g_World, XMMATRIX & g_View, XMMATRIX & g_Projection);
    void Release();
    ~WindMusic();
};
#endif WIND_MUSIC_FIGURE_H
