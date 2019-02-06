//--------------------------------------------------------------------------------------
// File: Tutorial07.cpp
//
// This application demonstrates texturing
//
// http://msdn.microsoft.com/en-us/library/windows/apps/ff729724.aspx
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <math.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "DDSTextureLoader.h"
#include "resource.h"
#define N 100
using namespace DirectX;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
};

/*struct CBNeverChanges
{
    XMMATRIX mView;
	XMVECTOR cameraPos;
};*/

struct CBChangeOnResize
{
    XMMATRIX mProjection;
};

struct CBChangesEveryFrame
{
    XMMATRIX mWorld;
	XMMATRIX mView;
	XMVECTOR cameraPos;
};

enum modes {COLOR, CUBEMAP, FRESNEL_COEF};

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE                           g_hInst = nullptr;
HWND                                g_hWnd = nullptr;
D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*                       g_pd3dDevice = nullptr;
ID3D11Device1*                      g_pd3dDevice1 = nullptr;
ID3D11DeviceContext*                g_pImmediateContext = nullptr;
ID3D11DeviceContext1*               g_pImmediateContext1 = nullptr;
IDXGISwapChain*                     g_pSwapChain = nullptr;
IDXGISwapChain1*                    g_pSwapChain1 = nullptr;
ID3D11RenderTargetView*             g_pRenderTargetView = nullptr;
ID3D11Texture2D*                    g_pDepthStencil = nullptr;
ID3D11DepthStencilView*             g_pDepthStencilView = nullptr;
ID3D11VertexShader*                 g_pVertexShader = nullptr;
ID3D11PixelShader*                  g_pPixelShaderColor = nullptr;
ID3D11PixelShader*                  g_pPixelShaderCubeMap = nullptr;
ID3D11PixelShader*                  g_pPixelShaderFresnelCoef3 = nullptr;
ID3D11PixelShader*                  g_pPixelShaderFresnelCoef8 = nullptr;
ID3D11InputLayout*                  g_pVertexLayout = nullptr;
ID3D11Buffer*                       g_pVertexBuffer = nullptr;
ID3D11Buffer*                       g_pIndexBuffer = nullptr;
ID3D11Buffer*                       g_pCBNeverChanges = nullptr;
ID3D11Buffer*                       g_pCBChangeOnResize = nullptr;
ID3D11Buffer*                       g_pCBChangesEveryFrame = nullptr;
ID3D11ShaderResourceView*           g_pTextureCubeMap = nullptr;
ID3D11SamplerState*                 g_pSampler = nullptr;
XMMATRIX                            g_World;
XMMATRIX                            g_View;
XMMATRIX                            g_Projection;
XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
bool first = true;
bool pause = false;

int mode = COLOR;


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Render();
        }
    }

    CleanupDevice();

    return ( int )msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 800, 600 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"TutorialWindowClass", L"Direct3D 11 Tutorial 7",
                           WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                           nullptr );
    if( !g_hWnd )
        return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile( szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob );
    if( FAILED(hr) )
    {
        if( pErrorBlob )
        {
            OutputDebugStringA( reinterpret_cast<const char*>( pErrorBlob->GetBufferPointer() ) );
            pErrorBlob->Release();
        }
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}

void countOddIndex(int i, int j, WORD index[], int* k, bool lowBorder, bool rightBorder)
{
	index[*k] = (WORD)(i * N + j);
	++(*k);
	index[*k] = (WORD)(i * N + (j + 1));
	if (rightBorder)
		index[*k] -= (WORD)(N);
	++(*k);
	index[*k] = (WORD)((i + 1) * N + j);
	if (lowBorder)
		index[*k] -= (WORD)(i + 1) * N;
	++(*k);
	index[*k] = (WORD)(i * N + (j + 1));
	if (rightBorder)
		index[*k] -= (WORD)(N);
	++(*k);
	index[*k] = (WORD)((i + 1) * N + (j + 1));
	if (rightBorder)
		index[*k] -= (WORD)(N);
	if (lowBorder)
		index[*k] -= (WORD)(i + 1) * N;
	++(*k);
	index[*k] = (WORD)((i + 1) * N + j);
	if (lowBorder)
		index[*k] -= (WORD)(i + 1) * N;
	++(*k);

}
void countEvenIndex(int i, int j, WORD index[], int* k, bool lowBorder, bool rightBorder)
{
	index[*k] = (WORD)(i * N + j);
	++(*k);
	index[*k] = (WORD)(i * N + (j + 1));
	if (rightBorder)
		index[*k] -= (WORD)(N);
	++(*k);
	index[*k] = (WORD)((i + 1) * N + (j + 1));
	if (rightBorder)
		index[*k] -= (WORD)(N);
	if (lowBorder)
		index[*k] -= (WORD)(i + 1) * N;
	++(*k);
	index[*k] = (WORD)(i * N + j);
	++(*k);
	index[*k] = (WORD)((i + 1) * N + (j + 1));
	if (rightBorder)
		index[*k] -= (WORD)(N);
	if (lowBorder)
		index[*k] -= (WORD)(i + 1) * N;
	++(*k);
	index[*k] = (WORD)((i + 1) * N + j);
	if (lowBorder)
		index[*k] -= (WORD)(i + 1) * N;
	++(*k);
}

void normalize(XMFLOAT3 * v) {
	float length = sqrtf(powf(v->x, 2) + powf(v->y, 2) + powf(v->z, 2));
	v = new XMFLOAT3(v->x / length, v->y / length, v->z / length);
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice( nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );

        if ( hr == E_INVALIDARG )
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice( nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                                    D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        }

        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = g_pd3dDevice->QueryInterface( __uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice) );
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent( __uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory) );
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    // Create swap chain
    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface( __uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2) );
    if ( dxgiFactory2 )
    {
        // DirectX 11.1 or later
        hr = g_pd3dDevice->QueryInterface( __uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1) );
        if (SUCCEEDED(hr))
        {
            (void) g_pImmediateContext->QueryInterface( __uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&g_pImmediateContext1) );
        }

        DXGI_SWAP_CHAIN_DESC1 sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd( g_pd3dDevice, g_hWnd, &sd, nullptr, nullptr, &g_pSwapChain1 );
        if (SUCCEEDED(hr))
        {
            hr = g_pSwapChain1->QueryInterface( __uuidof(IDXGISwapChain), reinterpret_cast<void**>(&g_pSwapChain) );
        }

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = g_hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain( g_pd3dDevice, &sd, &g_pSwapChain );
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation( g_hWnd, DXGI_MWA_NO_ALT_ENTER );

    dxgiFactory->Release();

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &pBackBuffer ) );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, nullptr, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, nullptr, &g_pDepthStencil );
    if( FAILED( hr ) )
        return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile( L"Tutorial07.fx", "VS", "vs_4_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( nullptr,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // Create the vertex shader
    hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader );
    if( FAILED( hr ) )
    {    
        pVSBlob->Release();
        return hr;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
    hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_pVertexLayout );
    pVSBlob->Release();
    if( FAILED( hr ) )
        return hr;

    // Set the input layout
    g_pImmediateContext->IASetInputLayout( g_pVertexLayout );

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile( L"Tutorial07.fx", "PS_Color", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( nullptr,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // Create the pixel shader
    hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShaderColor);
    pPSBlob->Release();
    if( FAILED( hr ) )
        return hr;

	// Compile the pixel shader
	pPSBlob = nullptr;
	hr = CompileShaderFromFile(L"Tutorial07.fx", "PS_CubeMap", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShaderCubeMap);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Compile the pixel shader
	pPSBlob = nullptr;
	hr = CompileShaderFromFile(L"Tutorial07.fx", "PS_Fresnel", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShaderFresnelCoef3);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	pPSBlob = nullptr;
	hr = CompileShaderFromFile(L"Tutorial07.fx", "PS_Fresnel8", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShaderFresnelCoef8);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Create vertex buffer
	SimpleVertex vertices[N*N];
	/*float step = 2 * XM_PI / N;
	int i = 0, j = 0;
	for (float phi = 0; phi < 2 * XM_PI; phi += step, i++) {
		j = 0;
		for (float psi = 0; psi <  2 * XM_PI; psi += step, j++) {
			float x = (Radius + radius * cosf(phi)) * cosf(psi);
			float y = (Radius + radius * cosf(phi)) * sinf(psi);
			float z = radius * sinf(phi);
			XMFLOAT3 v = XMFLOAT3(x, y, z);
			XMFLOAT3 center = XMFLOAT3((cosf(phi) * Radius), (sinf(phi) * Radius), 0.0f);
			XMFLOAT3 n = XMFLOAT3(v.x - center.x, v.y - center.y, v.z - center.z);
			normalize(&n);
			vertices[i * N + j] = { XMFLOAT3(x, y, z), center };

		}
	}*/
	float t = 0, f = 0, r = 0.75f;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			float x = r * sinf(t) * cosf(f);
			float y = r * sinf(t) * sinf(f);
			float z = r * cosf(t);
			XMFLOAT3 v = XMFLOAT3(x, y, z);
			XMFLOAT3 center = XMFLOAT3(cosf(t)* r,sinf(t)* r, 0.0f);
			XMFLOAT3 n = XMFLOAT3(v.x - center.x, v.y - center.y, v.z - center.z);
			normalize(&n);
			vertices[j + N * i] = { XMFLOAT3(x, z , y), XMFLOAT3(x, z, y) };
			f += XM_2PI / (N - 2);
		}
		t += XM_2PI / (N - 2);
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * N*N;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// Create index buffer
	WORD indices[N * N * 6];
/*	int k = 0;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			if ((i + j) % 2 == 1) {
				if (i == N - 1) {
					if (j == N - 1)
						countOddIndex(i, j, indices, &k, true, true);
					else
						countOddIndex(i, j, indices, &k, true, false);
				}
				else {
					if (j == N - 1)
						countOddIndex(i, j, indices, &k, false, true);
					else
						countOddIndex(i, j, indices, &k, false, false);
				}
			}
			else {
				if (i == N - 1) {
					if (j == N - 1)
						countEvenIndex(i, j, indices, &k, true, true);
					else
						countEvenIndex(i, j, indices, &k, true, false);
				}
				else {
					if (j == N - 1)
						countEvenIndex(i, j, indices, &k, false, true);
					else
						countEvenIndex(i, j, indices, &k, false, false);
				}
			}
		}
	}
	*/
	int k = 0, i = 0, j = 0;
	for (i = 0; i < N - 1; i++) {
		for (j = 0; j < N - 1; j++) {
			indices[k++] = (WORD)(j + i * N);
			indices[k++] = (WORD)(j + 1 + i * N);
			indices[k++] = (WORD)(j + 1 + N + i * N);
		}
		indices[k++] = (WORD)(j + i * N);
		indices[k++] = (WORD)(i * N);
		indices[k++] = (WORD)(i * N + N);
	}

	i = 1, j = 0;
	for (i = 1; i < N; i++) {
		for (j = 0; j < N - 1; j++) {
			indices[k++] = (WORD)(j + i * N);
			indices[k++] = (WORD)(i * N - N + j);
			indices[k++] = (WORD)(j + 1 + i * N);

		}
		indices[k++] = (WORD)(j + i * N);
		indices[k++] = (WORD)(i * N - 1);
		indices[k++] = (WORD)(i * N);
	}


	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * N * N * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
	if (FAILED(hr))
		return hr;

    // Set index buffer
    g_pImmediateContext->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

    // Set primitive topology
    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // Create the constant buffers
    bd.Usage = D3D11_USAGE_DEFAULT;
  //  bd.ByteWidth = sizeof(CBNeverChanges);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
   /* hr = g_pd3dDevice->CreateBuffer( &bd, nullptr, &g_pCBNeverChanges );
    if( FAILED( hr ) )
        return hr;*/
    
    bd.ByteWidth = sizeof(CBChangeOnResize);
    hr = g_pd3dDevice->CreateBuffer( &bd, nullptr, &g_pCBChangeOnResize );
    if( FAILED( hr ) )
        return hr;
    
    bd.ByteWidth = sizeof(CBChangesEveryFrame);
    hr = g_pd3dDevice->CreateBuffer( &bd, nullptr, &g_pCBChangesEveryFrame );
    if( FAILED( hr ) )
        return hr;

    // Load the Texture

	hr = CreateDDSTextureFromFile(g_pd3dDevice, L"bk.dds", nullptr, &g_pTextureCubeMap);
	if (FAILED(hr))
		return hr;

    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = g_pd3dDevice->CreateSamplerState( &sampDesc, &g_pSampler );
    if( FAILED( hr ) )
        return hr;

    // Initialize the world matrices
    g_World = XMMatrixIdentity();

    // Initialize the view matrix
    //XMVECTOR Eye = XMVectorSet( 0.0f, 3.0f, -6.0f, 0.0f );
   // XMVECTOR At = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
  //  XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
   // g_View = XMMatrixLookAtLH( Eye, At, Up );

	//CBNeverChanges cbNeverChanges;
	//cbNeverChanges.mView = XMMatrixTranspose(g_View);

	//XMMATRIX InvView = XMMatrixInverse(nullptr, g_View);
	//XMVECTOR cameraPos = InvView.r[3];
	//cbNeverChanges.cameraPos = cameraPos;

   // g_pImmediateContext->UpdateSubresource( g_pCBNeverChanges, 0, nullptr, &cbNeverChanges, 0, 0 );

    // Initialize the projection matrix
    g_Projection = XMMatrixPerspectiveFovLH( XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f );
    
    CBChangeOnResize cbChangesOnResize;
    cbChangesOnResize.mProjection = XMMatrixTranspose( g_Projection );
    g_pImmediateContext->UpdateSubresource( g_pCBChangeOnResize, 0, nullptr, &cbChangesOnResize, 0, 0 );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if( g_pImmediateContext ) g_pImmediateContext->ClearState();

	if (g_pTextureCubeMap) g_pTextureCubeMap->Release();
    if( g_pSampler ) g_pSampler->Release();
    if( g_pCBNeverChanges ) g_pCBNeverChanges->Release();
    if( g_pCBChangeOnResize ) g_pCBChangeOnResize->Release();
    if( g_pCBChangesEveryFrame ) g_pCBChangesEveryFrame->Release();
    if( g_pVertexBuffer ) g_pVertexBuffer->Release();
    if( g_pIndexBuffer ) g_pIndexBuffer->Release();
    if( g_pVertexLayout ) g_pVertexLayout->Release();
    if( g_pVertexShader ) g_pVertexShader->Release();
    if( g_pPixelShaderColor ) g_pPixelShaderColor->Release();
	if (g_pPixelShaderCubeMap) g_pPixelShaderCubeMap->Release();
	if (g_pPixelShaderFresnelCoef3) g_pPixelShaderFresnelCoef3->Release();
	if (g_pPixelShaderFresnelCoef8) g_pPixelShaderFresnelCoef8->Release();
    if( g_pDepthStencil ) g_pDepthStencil->Release();
    if( g_pDepthStencilView ) g_pDepthStencilView->Release();
    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain1 ) g_pSwapChain1->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pImmediateContext1 ) g_pImmediateContext1->Release();
    if( g_pImmediateContext ) g_pImmediateContext->Release();
    if( g_pd3dDevice1 ) g_pd3dDevice1->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();
}

XMVECTOR sphericalCoordinates(float r, float phi, float theta) {
	XMVECTOR eye;
	float cx, cy, cz;
	cx = r * cosf(theta) * sinf(phi);
	cy = r * cosf(phi);
	cz = r * sinf(theta) * sinf(phi);
	eye = XMVectorSet(cx, -cy, cz, 0.0f);
	return eye;

}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;
	int MAX_ZOOM = 15, MIN_ZOOM = 5;
	float MIN_PHI = XM_PI / 10, MAX_PHI = 9 * XM_PI / 10;
	static float zDelta, xPos, yPos, prevX, prevY, r = 5.0, theta = 3 * XM_PI / 2.0f, phi = XM_PI / 2.0f, cx, cy, cz;
	Eye = sphericalCoordinates(r, phi, theta);
    switch( message )
    {
    case WM_PAINT:
        hdc = BeginPaint( hWnd, &ps );
        EndPaint( hWnd, &ps );
        break;
	case WM_MOUSEWHEEL:
		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (r <= MAX_ZOOM && r >= MIN_ZOOM)
		{
			r += zDelta / 140.0f;
			if (zDelta > 0)
				r = min(MAX_ZOOM, r);
			else
				r = max(MIN_ZOOM, r);
		}
		Eye = sphericalCoordinates(r, phi, theta);
		break;
	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
		{
			xPos = LOWORD(lParam);
			yPos = HIWORD(lParam);
			if (first) {
				prevX = xPos;
				prevY = yPos;
				first = false;
			}
			else {
				if (xPos < 800 && xPos > 0 && yPos < 600 && yPos > 0) // check out screen
				{
					phi -= (prevY - yPos) / 50.0f;
					if (phi < MIN_PHI)
						phi = MIN_PHI;
					else if (phi > MAX_PHI)
						phi = MAX_PHI;
					theta += (prevX - xPos) / 50.0f;
					Eye = sphericalCoordinates(r, phi, theta);
				}
				prevX = xPos;//remember the previous coordinates
				prevY = yPos;
			}

		}
		else
			first = true;
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case('1'):
			mode = COLOR;
			break;
		case('2'):
			mode = CUBEMAP;
			break;
		case('3'):
			mode = FRESNEL_COEF;
			break;
		case ('P'):
			pause = (!pause);
		break;
		default:
			break;
		}
		break;
    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

        // Note that this tutorial does not handle resizing (WM_SIZE) requests,
        // so we created the window without the resize border.

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}


//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
	g_View = XMMatrixLookAtLH(Eye, At, Up);
	XMMATRIX InvView = XMMatrixInverse(nullptr, g_View);
	XMVECTOR cameraPos = InvView.r[3];
	
    // Update our time
	static float t = 0.0f;

	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		if (!pause)
			t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static ULONGLONG timeStart = 0;
		ULONGLONG timeCur = GetTickCount64();
		if (timeStart == 0)
			timeStart = timeCur;
		if (!pause)
			t += (timeCur - timeStart) / 1000.0f;
		timeStart = timeCur;
	}

    // Rotate cube around the origin
    g_World = XMMatrixRotationY( t );

    //
    // Clear the back buffer
    //
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, Colors::MidnightBlue );

    //
    // Clear the depth buffer to 1.0 (max depth)
    //
    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

    //
    // Update variables that change once per frame
    //
    CBChangesEveryFrame cb;
    cb.mWorld = XMMatrixTranspose( g_World );
	cb.mView = XMMatrixTranspose(g_View);
	cb.cameraPos = cameraPos;
    g_pImmediateContext->UpdateSubresource( g_pCBChangesEveryFrame, 0, nullptr, &cb, 0, 0 );


    g_pImmediateContext->VSSetShader( g_pVertexShader, nullptr, 0 );
    g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
    g_pImmediateContext->VSSetConstantBuffers( 1, 1, &g_pCBChangeOnResize );
    g_pImmediateContext->VSSetConstantBuffers( 2, 1, &g_pCBChangesEveryFrame );
	switch (mode) {
	case(COLOR):
		g_pImmediateContext->PSSetShader(g_pPixelShaderColor, nullptr, 0);
		break;
	case(CUBEMAP):
		g_pImmediateContext->PSSetShader(g_pPixelShaderCubeMap, nullptr, 0);
		break;
	case(FRESNEL_COEF):
		g_pImmediateContext->PSSetShader(g_pPixelShaderFresnelCoef3, nullptr, 0);
		break;
	}

    g_pImmediateContext->PSSetConstantBuffers( 2, 1, &g_pCBChangesEveryFrame );
    g_pImmediateContext->PSSetShaderResources( 0, 1, &g_pTextureCubeMap );
    g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSampler );
    g_pImmediateContext->DrawIndexed( N*N*6, 0, 0 );

	//second sphere
	cb.mWorld = XMMatrixTranspose(XMMatrixTranslation(-2, 0, 0));
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb, 0, 0);
	g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBNeverChanges);
	g_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pCBChangeOnResize);
	g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);
	switch (mode) {
	case(COLOR):
		g_pImmediateContext->PSSetShader(g_pPixelShaderColor, nullptr, 0);
		break;
	case(CUBEMAP):
		g_pImmediateContext->PSSetShader(g_pPixelShaderCubeMap, nullptr, 0);
		break;
	case(FRESNEL_COEF):
		g_pImmediateContext->PSSetShader(g_pPixelShaderFresnelCoef8, nullptr, 0);
		break;
	}

	g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCubeMap);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSampler);
	g_pImmediateContext->DrawIndexed(N*N * 6, 0, 0);

    //
    // Present our back buffer to our front buffer
    //
    g_pSwapChain->Present( 0, 0 );
}
