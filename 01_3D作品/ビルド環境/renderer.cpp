//=============================================================================
//
// レンダリング処理 [renderer.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "hp.h"
#include <d3d11sdklayers.h>  // デバッグ

//デバッグ用画面テキスト出力を有効にする
#define DEBUG_DISP_TEXTOUT
//シェーダーデバッグ設定を有効にする
//#define DEBUG_SHADER


//*********************************************************
// 構造体
//*********************************************************

// マテリアル用定数バッファ構造体
struct MATERIAL_CBUFFER
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];				// 16byte境界用
};

// ライト用フラグ構造体
struct LIGHTFLAGS
{
	int			Type;		//ライトタイプ（enum LIGHT_TYPE）
	int         OnOff;		//ライトのオンorオフスイッチ
	int			Dummy[2];
};

// ライト用定数バッファ構造体
struct LIGHT_CBUFFER
{
	XMFLOAT4	Direction[LIGHT_MAX];	// ライトの方向
	XMFLOAT4	Position[LIGHT_MAX];	// ライトの位置
	XMFLOAT4	Diffuse[LIGHT_MAX];		// 拡散光の色
	XMFLOAT4	Ambient[LIGHT_MAX];		// 環境光の色
	XMFLOAT4	Attenuation[LIGHT_MAX];	// 減衰率
	LIGHTFLAGS	Flags[LIGHT_MAX];		// ライト種別
	int			Enable;					// ライティング有効・無効フラグ
	int			Dummy[3];				// 16byte境界用
};

// フォグ用定数バッファ構造体
struct FOG_CBUFFER
{
	XMFLOAT4	Fog;					// フォグ量
	XMFLOAT4	FogColor;				// フォグの色
	int			Enable;					// フォグ有効・無効フラグ
	float		Dummy[3];				// 16byte境界用
};

// リムライト用バッファ
struct RIM
{
	int			rim;
	int			fill[3];
};

// ディゾルブ用バッファ
struct DISSOLVE_CBUFFER {
	XMFLOAT4 edgeColor;         // エッジの色

	int Enable;
	float dissolveThreshold;	// ディゾルブの閾値
	float edgeWidth;            // エッジの幅

	float Dummy[1];             //16byte境界用
};

// ダメージフィルター用バッファ
struct DAMAGE_CBUFFER
{
	float damageAlpha;
	float dummy[3];		// パディング合わせ
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static void SetLightBuffer(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static D3D_FEATURE_LEVEL       g_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

static ID3D11Device*           g_D3DDevice = NULL;
static ID3D11DeviceContext*    g_ImmediateContext = NULL;
static IDXGISwapChain*         g_SwapChain = NULL;
static ID3D11RenderTargetView* g_RenderTargetView = NULL;
static ID3D11DepthStencilView* g_DepthStencilView = NULL;
static ID3D11ShaderResourceView* g_NoiseTexture = NULL;		// ディゾルブ用


static ID3D11VertexShader*		g_VertexShader = NULL;
static ID3D11PixelShader*		g_PixelShader = NULL;
static ID3D11InputLayout*		g_VertexLayout = NULL;
static ID3D11Buffer*			g_WorldBuffer = NULL;
static ID3D11Buffer*			g_ViewBuffer = NULL;
static ID3D11Buffer*			g_ProjectionBuffer = NULL;
static ID3D11Buffer*			g_MaterialBuffer = NULL;
static ID3D11Buffer*			g_LightBuffer = NULL;
static ID3D11Buffer*			g_FogBuffer = NULL;
static ID3D11Buffer*			g_RimBuffer = NULL;
static ID3D11Buffer*			g_CameraBuffer = NULL;
static ID3D11Buffer*			g_LightPVMBuffer = NULL;	// シャドウの追加
static ID3D11Buffer*			g_DissolveBuffer = NULL;	// ディゾルブバッファ
static ID3D11Buffer*			g_DamageBuffer = NULL;		// ダメージフィルターバッファ


static ID3D11DepthStencilState* g_DepthStateEnable;
static ID3D11DepthStencilState* g_DepthStateDisable;

static ID3D11BlendState*		g_BlendStateNone;
static ID3D11BlendState*		g_BlendStateAlphaBlend;
static ID3D11BlendState*		g_BlendStateAdd;
static ID3D11BlendState*		g_BlendStateSubtract;
static BLEND_MODE				g_BlendStateParam;


static ID3D11RasterizerState*	g_RasterStateCullOff;
static ID3D11RasterizerState*	g_RasterStateCullCW;
static ID3D11RasterizerState*	g_RasterStateCullCCW;

// シャドウ
static ID3D11ShaderResourceView* ShadowMapSRV;
static ID3D11DepthStencilView*   ShadowMapDSV;
static ID3D11SamplerState*		 ShadowMap_samplerState;
static ID3D11VertexShader*		 ShadowMapVertexShader = NULL;

// シャドウ

static MATERIAL_CBUFFER	g_Material;
static LIGHT_CBUFFER	g_Light;
static FOG_CBUFFER		g_Fog;
//static TEXBLEND			g_Texblend;

static RIM			g_Rim;
static DISSOLVE_CBUFFER    g_Dissolve;	// ディゾルブ
static DAMAGE_CBUFFER      g_Damage;	// ダメージフィルター

static float g_ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };	// 背景色


ID3D11Device* GetDevice( void )
{
	return g_D3DDevice;
}


ID3D11DeviceContext* GetDeviceContext( void )
{
	return g_ImmediateContext;
}


void SetDepthEnable( BOOL Enable )
{
	if( Enable )
		g_ImmediateContext->OMSetDepthStencilState( g_DepthStateEnable, NULL );
	else
		g_ImmediateContext->OMSetDepthStencilState( g_DepthStateDisable, NULL );

}


void SetBlendState(BLEND_MODE bm)
{
	g_BlendStateParam = bm;

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	switch (g_BlendStateParam)
	{
	case BLEND_MODE_NONE:
		g_ImmediateContext->OMSetBlendState(g_BlendStateNone, blendFactor, 0xffffffff);
		break;
	case BLEND_MODE_ALPHABLEND:
		g_ImmediateContext->OMSetBlendState(g_BlendStateAlphaBlend, blendFactor, 0xffffffff);
		break;
	case BLEND_MODE_ADD:
		g_ImmediateContext->OMSetBlendState(g_BlendStateAdd, blendFactor, 0xffffffff);
		break;
	case BLEND_MODE_SUBTRACT:
		g_ImmediateContext->OMSetBlendState(g_BlendStateSubtract, blendFactor, 0xffffffff);
		break;
	}
}

void SetCullingMode(CULL_MODE cm)
{
	switch (cm)
	{
	case CULL_MODE_NONE:
		g_ImmediateContext->RSSetState(g_RasterStateCullOff);
		break;
	case CULL_MODE_FRONT:
		g_ImmediateContext->RSSetState(g_RasterStateCullCW);
		break;
	case CULL_MODE_BACK:
		g_ImmediateContext->RSSetState(g_RasterStateCullCCW);
		break;
	}
}

void SetAlphaTestEnable(BOOL flag)
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	if (flag)
		blendDesc.AlphaToCoverageEnable = TRUE;
	else
		blendDesc.AlphaToCoverageEnable = FALSE;

	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	switch (g_BlendStateParam)
	{
	case BLEND_MODE_NONE:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE_ALPHABLEND:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE_ADD:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE_SUBTRACT:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	}

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ID3D11BlendState* blendState = NULL;
	g_D3DDevice->CreateBlendState(&blendDesc, &blendState);
	g_ImmediateContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);

	if (blendState != NULL)
		blendState->Release();
}


void SetWorldViewProjection2D( void )
{
	XMMATRIX world;
	world = XMMatrixTranspose(XMMatrixIdentity());
	GetDeviceContext()->UpdateSubresource(g_WorldBuffer, 0, NULL, &world, 0, 0);

	XMMATRIX view;
	view = XMMatrixTranspose(XMMatrixIdentity());
	GetDeviceContext()->UpdateSubresource(g_ViewBuffer, 0, NULL, &view, 0, 0);

	XMMATRIX worldViewProjection;
	worldViewProjection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	worldViewProjection = XMMatrixTranspose(worldViewProjection);
	GetDeviceContext()->UpdateSubresource(g_ProjectionBuffer, 0, NULL, &worldViewProjection, 0, 0);
}


void SetWorldMatrix( XMMATRIX *WorldMatrix )
{
	XMMATRIX world;
	world = *WorldMatrix;
	world = XMMatrixTranspose(world);

	GetDeviceContext()->UpdateSubresource(g_WorldBuffer, 0, NULL, &world, 0, 0);
}

void SetViewMatrix(XMMATRIX *ViewMatrix )
{
	XMMATRIX view;
	view = *ViewMatrix;
	view = XMMatrixTranspose(view);

	GetDeviceContext()->UpdateSubresource(g_ViewBuffer, 0, NULL, &view, 0, 0);
}

void SetProjectionMatrix( XMMATRIX *ProjectionMatrix )
{
	XMMATRIX projection;
	projection = *ProjectionMatrix;
	projection = XMMatrixTranspose(projection);

	GetDeviceContext()->UpdateSubresource(g_ProjectionBuffer, 0, NULL, &projection, 0, 0);
}

void SetMaterial( MATERIAL material )
{
	g_Material.Diffuse = material.Diffuse;
	g_Material.Ambient = material.Ambient;
	g_Material.Specular = material.Specular;
	g_Material.Emission = material.Emission;
	g_Material.Shininess = material.Shininess;
	g_Material.noTexSampling = material.noTexSampling;

	GetDeviceContext()->UpdateSubresource( g_MaterialBuffer, 0, NULL, &g_Material, 0, 0 );
}

void SetLightBuffer(void)
{
	GetDeviceContext()->UpdateSubresource(g_LightBuffer, 0, NULL, &g_Light, 0, 0);
}

void SetLightEnable(BOOL flag)
{
	// フラグを更新する
	g_Light.Enable = flag;

	SetLightBuffer();
}

void SetLight(int index, LIGHT* pLight)
{
	g_Light.Position[index] = XMFLOAT4(pLight->Position.x, pLight->Position.y, pLight->Position.z, 0.0f);
	g_Light.Direction[index] = XMFLOAT4(pLight->Direction.x, pLight->Direction.y, pLight->Direction.z, 0.0f);
	g_Light.Diffuse[index] = pLight->Diffuse;
	g_Light.Ambient[index] = pLight->Ambient;
	g_Light.Flags[index].Type = pLight->Type;
	g_Light.Flags[index].OnOff = pLight->Enable;
	g_Light.Attenuation[index].x = pLight->Attenuation;

	SetLightBuffer();
}

void SetFogBuffer(void)
{
	GetDeviceContext()->UpdateSubresource(g_FogBuffer, 0, NULL, &g_Fog, 0, 0);
}

void SetFogEnable(BOOL flag)
{
	// フラグを更新する
	g_Fog.Enable = flag;

	SetFogBuffer();
}

void SetFog(FOG* pFog)
{
	g_Fog.Fog.x = pFog->FogStart;
	g_Fog.Fog.y = pFog->FogEnd;
	g_Fog.FogColor = pFog->FogColor;

	SetFogBuffer();
}

void SetRimLight(int flag)
{
	g_Rim.rim = flag;
	GetDeviceContext()->UpdateSubresource(g_RimBuffer, 0, NULL, &g_Rim, 0, 0);
}


void SetShaderCamera(XMFLOAT3 pos)
{
	XMFLOAT4 tmp = XMFLOAT4( pos.x, pos.y, pos.z, 0.0f );

	GetDeviceContext()->UpdateSubresource(g_CameraBuffer, 0, NULL, &tmp, 0, 0);
}



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	// デバイス、スワップチェーン、コンテキスト生成
	DWORD deviceFlags = 0;
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = SCREEN_WIDTH;
	sd.BufferDesc.Height = SCREEN_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = bWindow;

	//デバッグ文字出力用設定
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;
	deviceFlags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif

	hr = D3D11CreateDeviceAndSwapChain( NULL,
										D3D_DRIVER_TYPE_HARDWARE,
										NULL,
										deviceFlags,
										NULL,
										0,
										D3D11_SDK_VERSION,
										&sd,
										&g_SwapChain,
										&g_D3DDevice,
										&g_FeatureLevel,
										&g_ImmediateContext );
	if( FAILED( hr ) )
		return hr;

	//デバッグ文字出力用設定
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	hr = g_SwapChain->ResizeBuffers(0, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE); // N.B. the GDI compatible flag
	if (FAILED(hr))
		return hr;
#endif

	// レンダーターゲットビュー生成、設定
	ID3D11Texture2D* pBackBuffer = NULL;
	g_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	g_D3DDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_RenderTargetView );
	pBackBuffer->Release();



	//ステンシル用テクスチャー作成
	ID3D11Texture2D* depthTexture = NULL;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory( &td, sizeof(td) );
	td.Width			= sd.BufferDesc.Width;
	td.Height			= sd.BufferDesc.Height;
	td.MipLevels		= 1;
	td.ArraySize		= 1;
	td.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc		= sd.SampleDesc;
	td.Usage			= D3D11_USAGE_DEFAULT;
	td.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
    td.CPUAccessFlags	= 0;
    td.MiscFlags		= 0;
	g_D3DDevice->CreateTexture2D( &td, NULL, &depthTexture );

	//ステンシルターゲット作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory( &dsvd, sizeof(dsvd) );
	dsvd.Format			= td.Format;
	dsvd.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags			= 0;
	g_D3DDevice->CreateDepthStencilView( depthTexture, &dsvd, &g_DepthStencilView );

	depthTexture->Release();

	g_ImmediateContext->OMSetRenderTargets( 1, &g_RenderTargetView, g_DepthStencilView );


	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)SCREEN_WIDTH;
	vp.Height = (FLOAT)SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_ImmediateContext->RSSetViewports( 1, &vp );



	// ラスタライザステート作成
	D3D11_RASTERIZER_DESC rd; 
	ZeroMemory( &rd, sizeof( rd ) );
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE; 
	rd.DepthClipEnable = TRUE; 
	rd.MultisampleEnable = FALSE; 
	g_D3DDevice->CreateRasterizerState( &rd, &g_RasterStateCullOff);

	rd.CullMode = D3D11_CULL_FRONT;
	g_D3DDevice->CreateRasterizerState(&rd, &g_RasterStateCullCW);

	rd.CullMode = D3D11_CULL_BACK;
	g_D3DDevice->CreateRasterizerState(&rd, &g_RasterStateCullCCW);

	// カリングモード設定（CCW）
	SetCullingMode(CULL_MODE_BACK);



	// ブレンドステートの作成
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof( blendDesc ) );
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_D3DDevice->CreateBlendState( &blendDesc, &g_BlendStateAlphaBlend );

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_D3DDevice->CreateBlendState(&blendDesc, &g_BlendStateNone);

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_D3DDevice->CreateBlendState(&blendDesc, &g_BlendStateAdd);

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_D3DDevice->CreateBlendState(&blendDesc, &g_BlendStateSubtract);

	// アルファブレンド設定
	SetBlendState(BLEND_MODE_ALPHABLEND);


	// 深度ステンシルステート作成
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory( &depthStencilDesc, sizeof( depthStencilDesc ) );
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;

	g_D3DDevice->CreateDepthStencilState( &depthStencilDesc, &g_DepthStateEnable );//深度有効ステート

	//depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
	g_D3DDevice->CreateDepthStencilState( &depthStencilDesc, &g_DepthStateDisable );//深度無効ステート

	// 深度ステンシルステート設定
	SetDepthEnable(TRUE);


	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* samplerState = NULL;
	g_D3DDevice->CreateSamplerState( &samplerDesc, &samplerState );

	g_ImmediateContext->PSSetSamplers( 0, 1, &samplerState );

	// シャドウ
	CreateShadowMapResource();
	// ここでローカル samplerState を必ず解放
	samplerState->Release();

	// 頂点シェーダコンパイル・生成
	ID3DBlob* pErrorBlob;
	ID3DBlob* pVSBlob = NULL;
	DWORD shFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG) && defined(DEBUG_SHADER)
	shFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	hr = D3DX11CompileFromFile( "shader.hlsl", NULL, NULL, "VertexShaderPolygon", "vs_4_0", shFlag, 0, NULL, &pVSBlob, &pErrorBlob, NULL );
	if( FAILED(hr) )
	{
		MessageBox( NULL , (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR );
	}
	g_D3DDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_VertexShader );

	// 入力レイアウト生成
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE( layout );

	g_D3DDevice->CreateInputLayout( layout,
		numElements,
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&g_VertexLayout );

	pVSBlob->Release();
	if (pErrorBlob) 
	{
		pErrorBlob->Release();
		pErrorBlob = NULL;
	}
	// シャドウ（ここから）
	{
		hr = D3DX11CompileFromFile("shadowMap.hlsl", NULL, NULL, "VertexShaderShadowMap", "vs_4_0", shFlag, 0, NULL, &pVSBlob, &pErrorBlob, NULL);
		if (FAILED(hr))
		{
			MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR);
			if (pErrorBlob) pErrorBlob->Release();
		}
		g_D3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &ShadowMapVertexShader);
		pVSBlob->Release();
	}

	// ピクセルシェーダコンパイル・生成
	ID3DBlob* pPSBlob = NULL;
	hr = D3DX11CompileFromFile( "shader.hlsl", NULL, NULL, "PixelShaderPolygon", "ps_4_0", shFlag, 0, NULL, &pPSBlob, &pErrorBlob, NULL );
	if( FAILED(hr) )
	{
		MessageBox( NULL , (char*)pErrorBlob->GetBufferPointer(), "PS", MB_OK | MB_ICONERROR );
	}

	g_D3DDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_PixelShader );
	
	pPSBlob->Release();
	if (pErrorBlob)
	{
		pErrorBlob->Release();
		pErrorBlob = NULL;
	}
	// ノイズテクスチャを読み込む（ディゾルブ）
	hr = D3DX11CreateShaderResourceViewFromFile(
		g_D3DDevice,                // Direct3Dデバイス
		"data/TEXTURE/noise.png",    // ノイズテクスチャファイルのパス
		NULL,                        // 読み込みオプション（デフォルト）
		NULL,                        // 非同期読み込み用（デフォルト）
		&g_NoiseTexture,            // シェーダーリソースビュー
		NULL                        // エラーメッセージ（デフォルト）
	);

	if (FAILED(hr)) {
		MessageBox(NULL, "Failed to load noise texture", "Error", MB_OK | MB_ICONERROR);
	}

	g_ImmediateContext->PSSetShaderResources(4, 1, &g_NoiseTexture);


	// 定数バッファ生成
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = sizeof(XMMATRIX);
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	//ワールドマトリクス
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_WorldBuffer);
	g_ImmediateContext->VSSetConstantBuffers(0, 1, &g_WorldBuffer);
	g_ImmediateContext->PSSetConstantBuffers(0, 1, &g_WorldBuffer);

	//ビューマトリクス
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_ViewBuffer);
	g_ImmediateContext->VSSetConstantBuffers(1, 1, &g_ViewBuffer);
	g_ImmediateContext->PSSetConstantBuffers(1, 1, &g_ViewBuffer);

	//プロジェクションマトリクス
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_ProjectionBuffer);
	g_ImmediateContext->VSSetConstantBuffers(2, 1, &g_ProjectionBuffer);
	g_ImmediateContext->PSSetConstantBuffers(2, 1, &g_ProjectionBuffer);

	//マテリアル情報
	hBufferDesc.ByteWidth = sizeof(MATERIAL_CBUFFER);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_MaterialBuffer);
	g_ImmediateContext->VSSetConstantBuffers(3, 1, &g_MaterialBuffer);
	g_ImmediateContext->PSSetConstantBuffers(3, 1, &g_MaterialBuffer);

	//ライト情報
	hBufferDesc.ByteWidth = sizeof(LIGHT_CBUFFER);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_LightBuffer);
	g_ImmediateContext->VSSetConstantBuffers(4, 1, &g_LightBuffer);
	g_ImmediateContext->PSSetConstantBuffers(4, 1, &g_LightBuffer);

	//フォグ情報
	hBufferDesc.ByteWidth = sizeof(FOG_CBUFFER);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_FogBuffer);
	g_ImmediateContext->VSSetConstantBuffers(5, 1, &g_FogBuffer);
	g_ImmediateContext->PSSetConstantBuffers(5, 1, &g_FogBuffer);

	//リムライト
	hBufferDesc.ByteWidth = sizeof(RIM);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_RimBuffer);
	g_ImmediateContext->VSSetConstantBuffers(6, 1, &g_RimBuffer);
	g_ImmediateContext->PSSetConstantBuffers(6, 1, &g_RimBuffer);

	//カメラ
	hBufferDesc.ByteWidth = sizeof(XMFLOAT4);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_CameraBuffer);
	g_ImmediateContext->VSSetConstantBuffers(7, 1, &g_CameraBuffer);
	g_ImmediateContext->PSSetConstantBuffers(7, 1, &g_CameraBuffer);

	// シャドウ
	// Light PVM
	hBufferDesc.ByteWidth = sizeof(XMMATRIX);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_LightPVMBuffer);
	g_ImmediateContext->VSSetConstantBuffers(8, 1, &g_LightPVMBuffer);
	g_ImmediateContext->PSSetConstantBuffers(8, 1, &g_LightPVMBuffer);

	//ディゾルブ
	hBufferDesc.ByteWidth = sizeof(DISSOLVE_CBUFFER);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_DissolveBuffer);
	g_ImmediateContext->VSSetConstantBuffers(9, 1, &g_DissolveBuffer);
	g_ImmediateContext->PSSetConstantBuffers(9, 1, &g_DissolveBuffer);

	// ダメージフィルター用定数バッファ
	hBufferDesc.ByteWidth = sizeof(DAMAGE_CBUFFER);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL,  &g_DamageBuffer);
	g_ImmediateContext->VSSetConstantBuffers(10, 1, &g_DamageBuffer);
	g_ImmediateContext->PSSetConstantBuffers(10, 1, &g_DamageBuffer);

	// 入力レイアウト設定
	g_ImmediateContext->IASetInputLayout( g_VertexLayout );

	// シェーダ設定
	g_ImmediateContext->VSSetShader( g_VertexShader, NULL, 0 );
	g_ImmediateContext->PSSetShader( g_PixelShader, NULL, 0 );

	//ライト初期化
	ZeroMemory(&g_Light, sizeof(LIGHT_CBUFFER));
	g_Light.Direction[0] = XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f);
	g_Light.Diffuse[0] = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	g_Light.Ambient[0] = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	g_Light.Flags[0].Type = LIGHT_TYPE_DIRECTIONAL;
	SetLightBuffer();


	//マテリアル初期化
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	return S_OK;
}


//=============================================================================
// 終了処理
//=============================================================================
void UninitRenderer(void)
{
	// オブジェクト解放
	if (g_DepthStateEnable)		g_DepthStateEnable->Release();
	if (g_DepthStateDisable)	g_DepthStateDisable->Release();
	if (g_BlendStateNone)		g_BlendStateNone->Release();
	if (g_BlendStateAlphaBlend)	g_BlendStateAlphaBlend->Release();
	if (g_BlendStateAdd)		g_BlendStateAdd->Release();
	if (g_BlendStateSubtract)	g_BlendStateSubtract->Release();
	if (g_RasterStateCullOff)	g_RasterStateCullOff->Release();
	if (g_RasterStateCullCW)	g_RasterStateCullCW->Release();
	if (g_RasterStateCullCCW)	g_RasterStateCullCCW->Release();

	if (g_WorldBuffer)			g_WorldBuffer->Release();
	if (g_ViewBuffer)			g_ViewBuffer->Release();
	if (g_ProjectionBuffer)		g_ProjectionBuffer->Release();
	if (g_MaterialBuffer)		g_MaterialBuffer->Release();
	if (g_LightBuffer)			g_LightBuffer->Release();
	if (g_FogBuffer)			g_FogBuffer->Release();
	if (g_LightPVMBuffer)		g_LightPVMBuffer->Release();	// シャドウマップのリリース
	if (g_DissolveBuffer)		g_DissolveBuffer->Release();	// ディゾルブのリリース
	if (g_DamageBuffer)			g_DamageBuffer->Release();		// ダメージフィルターのリリース
	if (ShadowMap_samplerState) ShadowMap_samplerState->Release();
	if (g_NoiseTexture)			g_NoiseTexture->Release();
	if (g_RimBuffer)            g_RimBuffer->Release();
	if (g_CameraBuffer)         g_CameraBuffer->Release();

	if (ShadowMapDSV)          ShadowMapDSV->Release();
	if (ShadowMapSRV)          ShadowMapSRV->Release();
	if (ShadowMapVertexShader) ShadowMapVertexShader->Release();

	if (g_VertexLayout)			g_VertexLayout->Release();
	if (g_VertexShader)			g_VertexShader->Release();
	if (g_PixelShader)			g_PixelShader->Release();

	if (g_DepthStencilView)		g_DepthStencilView->Release();
	if (g_RenderTargetView)		g_RenderTargetView->Release();
	if (g_SwapChain)			g_SwapChain->Release();

	if (g_ImmediateContext)
	{
		g_ImmediateContext->ClearState();
		g_ImmediateContext->Flush();
		g_ImmediateContext->Release();
	}
	if (g_D3DDevice)			g_D3DDevice->Release();

// デバッグレイヤーのレポート
#ifdef _DEBUG
	if (g_D3DDevice)
	{
		ID3D11Debug* d3dDebug = nullptr;
		if (SUCCEEDED(g_D3DDevice->QueryInterface(
			__uuidof(ID3D11Debug), (void**)&d3dDebug)))
		{
			// 生き残っている D3D11 オブジェクトを詳細に表示
			d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			d3dDebug->Release();
		}
	}

#endif
}



//=============================================================================
// バックバッファクリア
//=============================================================================
void Clear(void)
{
	// ダメージフィルターの追加
	static float alpha = 0.0f;
	if (GetDamageFlg()) {
		// ダメージを受けたフレームだけ一気に alpha＝1.0 に
		alpha = 0.8f;
		SetDamageFlg(FALSE);   // フラグをリセット
	}
	// フェードアウト（1フレームにつき0.05ずつ減少）
	alpha = (alpha > 0.05f) ? (alpha - 0.05f) : 0.0f;

	// ダメージ用定数バッファ更新
	float dbuf[4] = { alpha, 0, 0, 0 };
	g_ImmediateContext->UpdateSubresource(g_DamageBuffer, 0, NULL, dbuf, 0, 0);
	// ダメージフィルターの追加

	// バックバッファクリア
	g_ImmediateContext->ClearRenderTargetView(g_RenderTargetView, g_ClearColor);
	g_ImmediateContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

}


void SetClearColor(float* color4)
{
	g_ClearColor[0] = color4[0];
	g_ClearColor[1] = color4[1];
	g_ClearColor[2] = color4[2];
	g_ClearColor[3] = color4[3];
}


//=============================================================================
// プレゼント
//=============================================================================
void Present(void)
{
	g_SwapChain->Present( 0, 0 );
}


//=============================================================================
// デバッグ用テキスト出力
//=============================================================================
void DebugTextOut(char* text, int x, int y)
{
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	HRESULT hr;

	//バックバッファからサーフェスを取得する
	IDXGISurface1* pBackSurface = NULL;
	hr = g_SwapChain->GetBuffer(0, __uuidof(IDXGISurface1), (void**)&pBackSurface);

	if (SUCCEEDED(hr))
	{
		//取得したサーフェスからデバイスコンテキストを取得する
		HDC hdc;
		hr = pBackSurface->GetDC(FALSE, &hdc);

		if (SUCCEEDED(hr))
		{
			//文字色を白に変更
			SetTextColor(hdc, RGB(255, 255, 255));
			//背景を透明に変更
			SetBkMode(hdc, TRANSPARENT);

			RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = SCREEN_WIDTH;
			rect.bottom = SCREEN_HEIGHT;

			//テキスト出力
			DrawText(hdc, text, (int)strlen(text), &rect, DT_LEFT);

			//デバイスコンテキストを解放する
			pBackSurface->ReleaseDC(NULL);
		}
		//サーフェスを解放する
		pBackSurface->Release();

		//レンダリングターゲットがリセットされるのでセットしなおす
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	}
#endif
}

void SetLightPVMBuffer(XMMATRIX* LightPVM)
{
	XMMATRIX mtx;
	mtx = *LightPVM;
	GetDeviceContext()->UpdateSubresource(g_LightPVMBuffer, 0, NULL, &mtx, 0, 0);
}


// シャドウ（ここから）
//=============================================================================
// シャドウマップ作成用関数
//=============================================================================
HRESULT CreateShadowMapResource() {

	ID3D11Texture2D* shadowMap = NULL;
	D3D11_TEXTURE2D_DESC ShadowMap_tex2dDesc;
	ZeroMemory(&ShadowMap_tex2dDesc, sizeof(ShadowMap_tex2dDesc));
	ShadowMap_tex2dDesc.Width = SHADOW_MAP_WIDTH;
	ShadowMap_tex2dDesc.Height = SHADOW_MAP_HEIGHT;
	ShadowMap_tex2dDesc.MipLevels = 1;
	ShadowMap_tex2dDesc.ArraySize = 1;
	ShadowMap_tex2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	ShadowMap_tex2dDesc.SampleDesc.Count = 1;
	ShadowMap_tex2dDesc.Usage = D3D11_USAGE_DEFAULT;
	ShadowMap_tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	ShadowMap_tex2dDesc.CPUAccessFlags = 0;
	ShadowMap_tex2dDesc.MiscFlags = 0;
	if (FAILED(g_D3DDevice->CreateTexture2D(&ShadowMap_tex2dDesc, nullptr, &shadowMap))) {
		return S_FALSE;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC ShadowMap_dsvDesc;
	ZeroMemory(&ShadowMap_dsvDesc, sizeof(ShadowMap_dsvDesc));
	ShadowMap_dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ShadowMap_dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	ShadowMap_dsvDesc.Texture2D.MipSlice = 0;

	if (FAILED(g_D3DDevice->CreateDepthStencilView(shadowMap, &ShadowMap_dsvDesc, &ShadowMapDSV))) {
		return S_FALSE;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC ShadowMap_srvDesc;
	ZeroMemory(&ShadowMap_srvDesc, sizeof(ShadowMap_srvDesc));
	ShadowMap_srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	ShadowMap_srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShadowMap_srvDesc.Texture2D.MostDetailedMip = 0;
	ShadowMap_srvDesc.Texture2D.MipLevels = 1;

	if (FAILED(g_D3DDevice->CreateShaderResourceView(shadowMap, &ShadowMap_srvDesc, &ShadowMapSRV))) {
		return S_FALSE;
	}

	// Shadow Mapping Sampler 
	D3D11_SAMPLER_DESC ShadowMap_samplerDesc;
	ZeroMemory(&ShadowMap_samplerDesc, sizeof(ShadowMap_samplerDesc));
	ShadowMap_samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	ShadowMap_samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	ShadowMap_samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	ShadowMap_samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	ShadowMap_samplerDesc.BorderColor[0] = 1.0f;
	ShadowMap_samplerDesc.BorderColor[1] = 1.0f;
	ShadowMap_samplerDesc.BorderColor[2] = 1.0f;
	ShadowMap_samplerDesc.BorderColor[3] = 1.0f;
	ShadowMap_samplerDesc.MinLOD = 0.0f;
	ShadowMap_samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	ShadowMap_samplerDesc.MipLODBias = 0.f;
	ShadowMap_samplerDesc.MaxAnisotropy = 0;

	ShadowMap_samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	ShadowMap_samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	//ShadowMap_samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;

	g_D3DDevice->CreateSamplerState(&ShadowMap_samplerDesc, &ShadowMap_samplerState);
	g_ImmediateContext->PSSetSamplers(1, 1, &ShadowMap_samplerState);
	// Shadow Map
	// shadowMap テクスチャ本体のリリース
	shadowMap->Release();

	return S_OK;

}

void SetRenderTargetView(int renderTarget) {
	ID3D11ShaderResourceView* nullSRVs[1] = { nullptr };
	switch (renderTarget) {
	case RENDER_MODE_NORMAL:
		g_ImmediateContext->PSSetShaderResources(1, 1, nullSRVs);
		g_ImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
		g_ImmediateContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		break;
	case RENDER_MODE_SHADOW_MAP:
		g_ImmediateContext->PSSetShaderResources(1, 1, nullSRVs);
		g_ImmediateContext->OMSetRenderTargets(0, nullptr, ShadowMapDSV);
		g_ImmediateContext->ClearDepthStencilView(ShadowMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
		break;
	}
}

void ClearDSV() {
	g_ImmediateContext->ClearDepthStencilView(ShadowMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


void SetShaderResource(int target)
{
	ID3D11ShaderResourceView* nullSRVs[1] = { nullptr };
	if (target == RENDER_MODE_SHADOW_MAP) {
		g_ImmediateContext->PSSetShaderResources(1, 1, &ShadowMapSRV);
	}
	else {
		g_ImmediateContext->PSSetShaderResources(1, 1, nullSRVs);
	}
}


void SetShader(int shaderMode) {
	switch (shaderMode) {
	case RENDER_MODE_NORMAL:
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShader, NULL, 0);
		break;

	case RENDER_MODE_SHADOW_MAP:
		g_ImmediateContext->VSSetShader(ShadowMapVertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(NULL, NULL, 0);
		break;
	}
}
// シャドウ（ここまで）
// ディゾルブ
void SetDissolveBuffer(int flag, float currentThreshold)
{

	g_Dissolve.Enable = flag;

	g_Dissolve.dissolveThreshold = currentThreshold; // 0.0～1.0でアニメーション
	g_Dissolve.edgeWidth = 0.05f;

	GetDeviceContext()->UpdateSubresource(g_DissolveBuffer, 0, NULL, &g_Dissolve, 0, 0);
}

void SetDissolveDiffuse(DISSOLVE* dissolve)
{
	g_Dissolve.edgeColor = dissolve->Diffuse; // エッジの色調整
}
