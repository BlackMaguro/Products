//=============================================================================
//
// �����_�����O���� [renderer.cpp]
// Author : ����z��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "hp.h"
#include <d3d11sdklayers.h>  // �f�o�b�O

//�f�o�b�O�p��ʃe�L�X�g�o�͂�L���ɂ���
#define DEBUG_DISP_TEXTOUT
//�V�F�[�_�[�f�o�b�O�ݒ��L���ɂ���
//#define DEBUG_SHADER


//*********************************************************
// �\����
//*********************************************************

// �}�e���A���p�萔�o�b�t�@�\����
struct MATERIAL_CBUFFER
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];				// 16byte���E�p
};

// ���C�g�p�t���O�\����
struct LIGHTFLAGS
{
	int			Type;		//���C�g�^�C�v�ienum LIGHT_TYPE�j
	int         OnOff;		//���C�g�̃I��or�I�t�X�C�b�`
	int			Dummy[2];
};

// ���C�g�p�萔�o�b�t�@�\����
struct LIGHT_CBUFFER
{
	XMFLOAT4	Direction[LIGHT_MAX];	// ���C�g�̕���
	XMFLOAT4	Position[LIGHT_MAX];	// ���C�g�̈ʒu
	XMFLOAT4	Diffuse[LIGHT_MAX];		// �g�U���̐F
	XMFLOAT4	Ambient[LIGHT_MAX];		// �����̐F
	XMFLOAT4	Attenuation[LIGHT_MAX];	// ������
	LIGHTFLAGS	Flags[LIGHT_MAX];		// ���C�g���
	int			Enable;					// ���C�e�B���O�L���E�����t���O
	int			Dummy[3];				// 16byte���E�p
};

// �t�H�O�p�萔�o�b�t�@�\����
struct FOG_CBUFFER
{
	XMFLOAT4	Fog;					// �t�H�O��
	XMFLOAT4	FogColor;				// �t�H�O�̐F
	int			Enable;					// �t�H�O�L���E�����t���O
	float		Dummy[3];				// 16byte���E�p
};

// �������C�g�p�o�b�t�@
struct RIM
{
	int			rim;
	int			fill[3];
};

// �f�B�]���u�p�o�b�t�@
struct DISSOLVE_CBUFFER {
	XMFLOAT4 edgeColor;         // �G�b�W�̐F

	int Enable;
	float dissolveThreshold;	// �f�B�]���u��臒l
	float edgeWidth;            // �G�b�W�̕�

	float Dummy[1];             //16byte���E�p
};

// �_���[�W�t�B���^�[�p�o�b�t�@
struct DAMAGE_CBUFFER
{
	float damageAlpha;
	float dummy[3];		// �p�f�B���O���킹
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
static void SetLightBuffer(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static D3D_FEATURE_LEVEL       g_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

static ID3D11Device*           g_D3DDevice = NULL;
static ID3D11DeviceContext*    g_ImmediateContext = NULL;
static IDXGISwapChain*         g_SwapChain = NULL;
static ID3D11RenderTargetView* g_RenderTargetView = NULL;
static ID3D11DepthStencilView* g_DepthStencilView = NULL;
static ID3D11ShaderResourceView* g_NoiseTexture = NULL;		// �f�B�]���u�p


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
static ID3D11Buffer*			g_LightPVMBuffer = NULL;	// �V���h�E�̒ǉ�
static ID3D11Buffer*			g_DissolveBuffer = NULL;	// �f�B�]���u�o�b�t�@
static ID3D11Buffer*			g_DamageBuffer = NULL;		// �_���[�W�t�B���^�[�o�b�t�@


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

// �V���h�E
static ID3D11ShaderResourceView* ShadowMapSRV;
static ID3D11DepthStencilView*   ShadowMapDSV;
static ID3D11SamplerState*		 ShadowMap_samplerState;
static ID3D11VertexShader*		 ShadowMapVertexShader = NULL;

// �V���h�E

static MATERIAL_CBUFFER	g_Material;
static LIGHT_CBUFFER	g_Light;
static FOG_CBUFFER		g_Fog;
//static TEXBLEND			g_Texblend;

static RIM			g_Rim;
static DISSOLVE_CBUFFER    g_Dissolve;	// �f�B�]���u
static DAMAGE_CBUFFER      g_Damage;	// �_���[�W�t�B���^�[

static float g_ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };	// �w�i�F


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
	// �t���O���X�V����
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
	// �t���O���X�V����
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
// ����������
//=============================================================================
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	// �f�o�C�X�A�X���b�v�`�F�[���A�R���e�L�X�g����
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

	//�f�o�b�O�����o�͗p�ݒ�
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

	//�f�o�b�O�����o�͗p�ݒ�
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	hr = g_SwapChain->ResizeBuffers(0, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE); // N.B. the GDI compatible flag
	if (FAILED(hr))
		return hr;
#endif

	// �����_�[�^�[�Q�b�g�r���[�����A�ݒ�
	ID3D11Texture2D* pBackBuffer = NULL;
	g_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	g_D3DDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_RenderTargetView );
	pBackBuffer->Release();



	//�X�e���V���p�e�N�X�`���[�쐬
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

	//�X�e���V���^�[�Q�b�g�쐬
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory( &dsvd, sizeof(dsvd) );
	dsvd.Format			= td.Format;
	dsvd.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags			= 0;
	g_D3DDevice->CreateDepthStencilView( depthTexture, &dsvd, &g_DepthStencilView );

	depthTexture->Release();

	g_ImmediateContext->OMSetRenderTargets( 1, &g_RenderTargetView, g_DepthStencilView );


	// �r���[�|�[�g�ݒ�
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)SCREEN_WIDTH;
	vp.Height = (FLOAT)SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_ImmediateContext->RSSetViewports( 1, &vp );



	// ���X�^���C�U�X�e�[�g�쐬
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

	// �J�����O���[�h�ݒ�iCCW�j
	SetCullingMode(CULL_MODE_BACK);



	// �u�����h�X�e�[�g�̍쐬
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

	// �A���t�@�u�����h�ݒ�
	SetBlendState(BLEND_MODE_ALPHABLEND);


	// �[�x�X�e���V���X�e�[�g�쐬
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory( &depthStencilDesc, sizeof( depthStencilDesc ) );
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;

	g_D3DDevice->CreateDepthStencilState( &depthStencilDesc, &g_DepthStateEnable );//�[�x�L���X�e�[�g

	//depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
	g_D3DDevice->CreateDepthStencilState( &depthStencilDesc, &g_DepthStateDisable );//�[�x�����X�e�[�g

	// �[�x�X�e���V���X�e�[�g�ݒ�
	SetDepthEnable(TRUE);


	// �T���v���[�X�e�[�g�ݒ�
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

	// �V���h�E
	CreateShadowMapResource();
	// �����Ń��[�J�� samplerState ��K�����
	samplerState->Release();

	// ���_�V�F�[�_�R���p�C���E����
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

	// ���̓��C�A�E�g����
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
	// �V���h�E�i��������j
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

	// �s�N�Z���V�F�[�_�R���p�C���E����
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
	// �m�C�Y�e�N�X�`����ǂݍ��ށi�f�B�]���u�j
	hr = D3DX11CreateShaderResourceViewFromFile(
		g_D3DDevice,                // Direct3D�f�o�C�X
		"data/TEXTURE/noise.png",    // �m�C�Y�e�N�X�`���t�@�C���̃p�X
		NULL,                        // �ǂݍ��݃I�v�V�����i�f�t�H���g�j
		NULL,                        // �񓯊��ǂݍ��ݗp�i�f�t�H���g�j
		&g_NoiseTexture,            // �V�F�[�_�[���\�[�X�r���[
		NULL                        // �G���[���b�Z�[�W�i�f�t�H���g�j
	);

	if (FAILED(hr)) {
		MessageBox(NULL, "Failed to load noise texture", "Error", MB_OK | MB_ICONERROR);
	}

	g_ImmediateContext->PSSetShaderResources(4, 1, &g_NoiseTexture);


	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = sizeof(XMMATRIX);
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	//���[���h�}�g���N�X
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_WorldBuffer);
	g_ImmediateContext->VSSetConstantBuffers(0, 1, &g_WorldBuffer);
	g_ImmediateContext->PSSetConstantBuffers(0, 1, &g_WorldBuffer);

	//�r���[�}�g���N�X
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_ViewBuffer);
	g_ImmediateContext->VSSetConstantBuffers(1, 1, &g_ViewBuffer);
	g_ImmediateContext->PSSetConstantBuffers(1, 1, &g_ViewBuffer);

	//�v���W�F�N�V�����}�g���N�X
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_ProjectionBuffer);
	g_ImmediateContext->VSSetConstantBuffers(2, 1, &g_ProjectionBuffer);
	g_ImmediateContext->PSSetConstantBuffers(2, 1, &g_ProjectionBuffer);

	//�}�e���A�����
	hBufferDesc.ByteWidth = sizeof(MATERIAL_CBUFFER);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_MaterialBuffer);
	g_ImmediateContext->VSSetConstantBuffers(3, 1, &g_MaterialBuffer);
	g_ImmediateContext->PSSetConstantBuffers(3, 1, &g_MaterialBuffer);

	//���C�g���
	hBufferDesc.ByteWidth = sizeof(LIGHT_CBUFFER);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_LightBuffer);
	g_ImmediateContext->VSSetConstantBuffers(4, 1, &g_LightBuffer);
	g_ImmediateContext->PSSetConstantBuffers(4, 1, &g_LightBuffer);

	//�t�H�O���
	hBufferDesc.ByteWidth = sizeof(FOG_CBUFFER);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_FogBuffer);
	g_ImmediateContext->VSSetConstantBuffers(5, 1, &g_FogBuffer);
	g_ImmediateContext->PSSetConstantBuffers(5, 1, &g_FogBuffer);

	//�������C�g
	hBufferDesc.ByteWidth = sizeof(RIM);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_RimBuffer);
	g_ImmediateContext->VSSetConstantBuffers(6, 1, &g_RimBuffer);
	g_ImmediateContext->PSSetConstantBuffers(6, 1, &g_RimBuffer);

	//�J����
	hBufferDesc.ByteWidth = sizeof(XMFLOAT4);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_CameraBuffer);
	g_ImmediateContext->VSSetConstantBuffers(7, 1, &g_CameraBuffer);
	g_ImmediateContext->PSSetConstantBuffers(7, 1, &g_CameraBuffer);

	// �V���h�E
	// Light PVM
	hBufferDesc.ByteWidth = sizeof(XMMATRIX);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_LightPVMBuffer);
	g_ImmediateContext->VSSetConstantBuffers(8, 1, &g_LightPVMBuffer);
	g_ImmediateContext->PSSetConstantBuffers(8, 1, &g_LightPVMBuffer);

	//�f�B�]���u
	hBufferDesc.ByteWidth = sizeof(DISSOLVE_CBUFFER);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_DissolveBuffer);
	g_ImmediateContext->VSSetConstantBuffers(9, 1, &g_DissolveBuffer);
	g_ImmediateContext->PSSetConstantBuffers(9, 1, &g_DissolveBuffer);

	// �_���[�W�t�B���^�[�p�萔�o�b�t�@
	hBufferDesc.ByteWidth = sizeof(DAMAGE_CBUFFER);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL,  &g_DamageBuffer);
	g_ImmediateContext->VSSetConstantBuffers(10, 1, &g_DamageBuffer);
	g_ImmediateContext->PSSetConstantBuffers(10, 1, &g_DamageBuffer);

	// ���̓��C�A�E�g�ݒ�
	g_ImmediateContext->IASetInputLayout( g_VertexLayout );

	// �V�F�[�_�ݒ�
	g_ImmediateContext->VSSetShader( g_VertexShader, NULL, 0 );
	g_ImmediateContext->PSSetShader( g_PixelShader, NULL, 0 );

	//���C�g������
	ZeroMemory(&g_Light, sizeof(LIGHT_CBUFFER));
	g_Light.Direction[0] = XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f);
	g_Light.Diffuse[0] = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	g_Light.Ambient[0] = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	g_Light.Flags[0].Type = LIGHT_TYPE_DIRECTIONAL;
	SetLightBuffer();


	//�}�e���A��������
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	return S_OK;
}


//=============================================================================
// �I������
//=============================================================================
void UninitRenderer(void)
{
	// �I�u�W�F�N�g���
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
	if (g_LightPVMBuffer)		g_LightPVMBuffer->Release();	// �V���h�E�}�b�v�̃����[�X
	if (g_DissolveBuffer)		g_DissolveBuffer->Release();	// �f�B�]���u�̃����[�X
	if (g_DamageBuffer)			g_DamageBuffer->Release();		// �_���[�W�t�B���^�[�̃����[�X
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

// �f�o�b�O���C���[�̃��|�[�g
#ifdef _DEBUG
	if (g_D3DDevice)
	{
		ID3D11Debug* d3dDebug = nullptr;
		if (SUCCEEDED(g_D3DDevice->QueryInterface(
			__uuidof(ID3D11Debug), (void**)&d3dDebug)))
		{
			// �����c���Ă��� D3D11 �I�u�W�F�N�g���ڍׂɕ\��
			d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			d3dDebug->Release();
		}
	}

#endif
}



//=============================================================================
// �o�b�N�o�b�t�@�N���A
//=============================================================================
void Clear(void)
{
	// �_���[�W�t�B���^�[�̒ǉ�
	static float alpha = 0.0f;
	if (GetDamageFlg()) {
		// �_���[�W���󂯂��t���[��������C�� alpha��1.0 ��
		alpha = 0.8f;
		SetDamageFlg(FALSE);   // �t���O�����Z�b�g
	}
	// �t�F�[�h�A�E�g�i1�t���[���ɂ�0.05�������j
	alpha = (alpha > 0.05f) ? (alpha - 0.05f) : 0.0f;

	// �_���[�W�p�萔�o�b�t�@�X�V
	float dbuf[4] = { alpha, 0, 0, 0 };
	g_ImmediateContext->UpdateSubresource(g_DamageBuffer, 0, NULL, dbuf, 0, 0);
	// �_���[�W�t�B���^�[�̒ǉ�

	// �o�b�N�o�b�t�@�N���A
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
// �v���[���g
//=============================================================================
void Present(void)
{
	g_SwapChain->Present( 0, 0 );
}


//=============================================================================
// �f�o�b�O�p�e�L�X�g�o��
//=============================================================================
void DebugTextOut(char* text, int x, int y)
{
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	HRESULT hr;

	//�o�b�N�o�b�t�@����T�[�t�F�X���擾����
	IDXGISurface1* pBackSurface = NULL;
	hr = g_SwapChain->GetBuffer(0, __uuidof(IDXGISurface1), (void**)&pBackSurface);

	if (SUCCEEDED(hr))
	{
		//�擾�����T�[�t�F�X����f�o�C�X�R���e�L�X�g���擾����
		HDC hdc;
		hr = pBackSurface->GetDC(FALSE, &hdc);

		if (SUCCEEDED(hr))
		{
			//�����F�𔒂ɕύX
			SetTextColor(hdc, RGB(255, 255, 255));
			//�w�i�𓧖��ɕύX
			SetBkMode(hdc, TRANSPARENT);

			RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = SCREEN_WIDTH;
			rect.bottom = SCREEN_HEIGHT;

			//�e�L�X�g�o��
			DrawText(hdc, text, (int)strlen(text), &rect, DT_LEFT);

			//�f�o�C�X�R���e�L�X�g���������
			pBackSurface->ReleaseDC(NULL);
		}
		//�T�[�t�F�X���������
		pBackSurface->Release();

		//�����_�����O�^�[�Q�b�g�����Z�b�g�����̂ŃZ�b�g���Ȃ���
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


// �V���h�E�i��������j
//=============================================================================
// �V���h�E�}�b�v�쐬�p�֐�
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
	// shadowMap �e�N�X�`���{�̂̃����[�X
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
// �V���h�E�i�����܂Łj
// �f�B�]���u
void SetDissolveBuffer(int flag, float currentThreshold)
{

	g_Dissolve.Enable = flag;

	g_Dissolve.dissolveThreshold = currentThreshold; // 0.0�`1.0�ŃA�j���[�V����
	g_Dissolve.edgeWidth = 0.05f;

	GetDeviceContext()->UpdateSubresource(g_DissolveBuffer, 0, NULL, &g_Dissolve, 0, 0);
}

void SetDissolveDiffuse(DISSOLVE* dissolve)
{
	g_Dissolve.edgeColor = dissolve->Diffuse; // �G�b�W�̐F����
}
