//=============================================================================
//
// レンダリング処理 [renderer.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once


//*********************************************************
// マクロ定義
//*********************************************************
#define LIGHT_MAX		(10)
//#define SHADOW_MAP_WIDTH		(1024)
//#define SHADOW_MAP_HEIGHT		(1024)
#define SHADOW_MAP_WIDTH		(4096)
#define SHADOW_MAP_HEIGHT		(4096)

enum LIGHT_TYPE
{
	LIGHT_TYPE_NONE,		//ライト無し
	LIGHT_TYPE_DIRECTIONAL,	//ディレクショナルライト
	LIGHT_TYPE_POINT,		//ポイントライト

	LIGHT_TYPE_NUM
};

enum BLEND_MODE
{
	BLEND_MODE_NONE,		//ブレンド無し
	BLEND_MODE_ALPHABLEND,	//αブレンド
	BLEND_MODE_ADD,			//加算ブレンド
	BLEND_MODE_SUBTRACT,	//減算ブレンド

	BLEDD_MODE_NUM
};

enum CULL_MODE
{
	CULL_MODE_NONE,			//カリング無し
	CULL_MODE_FRONT,		//表のポリゴンを描画しない(CW)
	CULL_MODE_BACK,			//裏のポリゴンを描画しない(CCW)

	CULL_MODE_NUM
};

enum RENDER_MODE
{
	RENDER_MODE_NORMAL,			// シャドウマップなし
	RENDER_MODE_SHADOW_MAP,		// シャドウマップあり

	RENDER_MODE_NUM
};


//*********************************************************
// 構造体
//*********************************************************

// 頂点構造体
struct VERTEX_3D
{
    XMFLOAT3	Position;
    XMFLOAT3	Normal;
    XMFLOAT4	Diffuse;
    XMFLOAT2	TexCoord;
};

// マテリアル構造体
struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	int			noTexSampling;
};

// ライト構造体
struct LIGHT {
	XMFLOAT3	Direction;	// ライトの方向
	XMFLOAT3	Position;	// ライトの位置
	XMFLOAT4	Diffuse;	// 拡散光の色
	XMFLOAT4	Ambient;	// 環境光の色
	float		Attenuation;// 減衰率
	int			Type;		// ライト種別・有効フラグ
	int			Enable;		// ライト種別・有効フラグ
};

// フォグ構造体
struct FOG {
	float		FogStart;	// フォグの開始距離
	float		FogEnd;		// フォグの最大距離
	XMFLOAT4	FogColor;	// フォグの色
};

// ディゾルブ構造体
struct DISSOLVE
{
	XMFLOAT4 Diffuse;		// エッジの色
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void UninitRenderer(void);

void Clear(void);
void Present(void);

ID3D11Device *GetDevice( void );
ID3D11DeviceContext *GetDeviceContext( void );

void SetDepthEnable( BOOL Enable );
void SetBlendState(BLEND_MODE bm);
void SetCullingMode(CULL_MODE cm);
void SetAlphaTestEnable(BOOL flag);

void SetWorldViewProjection2D( void );
void SetWorldMatrix( XMMATRIX *WorldMatrix );
void SetViewMatrix( XMMATRIX *ViewMatrix );
void SetProjectionMatrix( XMMATRIX *ProjectionMatrix );

void SetMaterial( MATERIAL material );

void SetLightEnable(BOOL flag);
void SetLight(int index, LIGHT* light);

void SetFogEnable(BOOL flag);
void SetFog(FOG* fog);

void DebugTextOut(char* text, int x, int y);

void SetRimLight(int flag);
void SetShaderCamera(XMFLOAT3 pos);

void SetClearColor(float* color4);
void RenderShadowMap();

// シャドウ（ここから）
HRESULT CreateShadowMapResource();
void ClearDSV();
void SetRenderTargetView(int renderTarget);
void SetLightPVMBuffer(XMMATRIX* LightPVM);
void SetShaderResource(int target);
void SetShader(int shaderMode);
// シャドウ（ここまで）

// テクスチャブレンド
void SetTexBrend(int flug, float rate, XMFLOAT2 time);

// ディゾルブ
void SetDissolveBuffer(int flag, float currentThreshold);
void SetDissolveDiffuse(DISSOLVE* dissolve);
