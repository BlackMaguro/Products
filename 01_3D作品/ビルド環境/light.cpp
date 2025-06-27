//=============================================================================
//
// ライト処理 [light.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;

static BOOL		g_FogEnable = TRUE;		// Fogのオンオフ
//static BOOL		g_FogEnable = FALSE;		// Fogのオンオフ

// ライトの設定
static XMMATRIX g_LightViewMatrix;
static XMMATRIX g_LightProjectionMatrix;

// シャドウ（ここから）
static 	XMFLOAT3 g_pos = { 0.0f, 10.0f,0.0f };
static 	XMFLOAT3 g_dir = { -2.5f, -1.0f,2.5f };
// シャドウ（ここまで）

//=============================================================================
// 初期化処理
//=============================================================================
void InitLight(void)
{

	//ライト初期化
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position  = XMFLOAT3( 0.0f, 0.0f, 0.0f );
		g_Light[i].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );
		g_Light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		g_Light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		g_Light[i].Attenuation = 100.0f;	// 減衰距離
		g_Light[i].Type = LIGHT_TYPE_NONE;	// ライトのタイプ
		g_Light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}

	// 並行光源の設定（世界を照らす光）
	g_Light[0].Direction = XMFLOAT3( -1.0f, -1.0f, 0.2f );			// 光の向き
	g_Light[0].Diffuse   = XMFLOAT4( 1.5f, 1.5f, 1.5f, 1.0f );		// 光の色
	//g_Light[0].Diffuse   = XMFLOAT4( 0.5f, 0.5f, 0.5f, 1.0f );	// 光の色
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;						// 並行光源
	g_Light[0].Enable = TRUE;										// このライトをON
	SetLight(0, &g_Light[0]);										// ライトの設定



	// フォグの初期化（霧の効果）
	//g_Fog.FogStart = 350.0f;									// 視点からこの距離離れるとフォグがかかり始める
	g_Fog.FogStart = 100.0f;									// 視点からこの距離離れるとフォグがかかり始める
	g_Fog.FogEnd   = 800.0f;									// ここまで離れるとフォグの色で見えなくなる
	g_Fog.FogColor = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.6f );		// フォグの色
	SetFog(&g_Fog);
	SetFogEnable(g_FogEnable);				// 他の場所もチェックする shadow

}


//=============================================================================
// 更新処理
//=============================================================================
void UpdateLight(void)
{
	static float angle = 0.0f;
	// フレームごとに少しずつ回転させる（2πを何秒で回すかを調整）
	const float speed = 0.005f;    // 値を大きくすると速くなる
	angle += speed;
	if (angle > XM_2PI) angle -= XM_2PI;

	// 傾き（仰角）を固定
	const float tilt = XM_PI / 12.0f;
	float cosT = cosf(tilt);
	float sinT = sinf(tilt);

	// 方向ベクトルを計算
	XMFLOAT3 dir = {
		-cosT * cosf(angle),  // X
		-sinT,                // Y 
		-cosT * sinf(angle)   // Z
	};

	// 正規化してセット
	XMVECTOR vec = XMLoadFloat3(&dir);
	vec = XMVector3Normalize(vec);
	XMStoreFloat3(&g_Light[0].Direction, vec);

	SetLight(0, &g_Light[0]);
}


//=============================================================================
// ライトの設定
// Typeによってセットするメンバー変数が変わってくる
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}


LIGHT *GetLightData(int index)
{
	return(&g_Light[index]);
}


//=============================================================================
// フォグの設定
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}


BOOL GetFogEnable(void)
{
	return(g_FogEnable);
}

//=============================================================================
// シャドウの設定
//=============================================================================
void SetLightProjectViewMatrix()
{
	PLAYER* player = GetPlayer();
	g_dir = g_Light[0].Direction;

	XMFLOAT3 p_pos = player->pos;
	g_pos = player->pos;

	XMFLOAT3 up = { 0.0f,1.0f,0.0f };
	//XMMATRIX lightView;

	XMVECTOR  l_dir = XMVector3Normalize(XMLoadFloat3(&g_dir));
	float lightHeight = 500.0f;
	XMVECTOR player_pos = XMLoadFloat3(&p_pos);
	XMVECTOR lightPosition = player_pos - l_dir * lightHeight;

	XMMATRIX lightView = XMMatrixLookAtLH(lightPosition, XMLoadFloat3(&p_pos), XMLoadFloat3(&up));
	XMMATRIX lightProjection = XMMatrixOrthographicLH(1000.0f, 1000.0f, 0.1f, 1000.0f);
	XMMATRIX lightProjView = XMMatrixMultiply(lightView, lightProjection);
	lightProjView = XMMatrixTranspose(lightProjView);

	SetLightPVMBuffer(&lightProjView);

}

// ベクトル正規化関数
XMFLOAT3 Normalize(XMFLOAT3 v)
{
	XMVECTOR vec = XMLoadFloat3(&v);
	vec = XMVector3Normalize(vec);
	XMFLOAT3 result;
	XMStoreFloat3(&result, vec);
	return result;
}

