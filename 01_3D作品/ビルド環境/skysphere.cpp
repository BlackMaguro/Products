//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "skysphere.h"
#include "debugproc.h"
#include "shadow.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_SKYSPHERE		"data/MODEL/02_SKYSPHERE/skySphere.obj"		// 読み込むモデル名
#define SPHERE_SIZE			(100)						// 
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static SKYSPHERE g_SkySphere;				// スカイスフィア
int g_SkySphere_load = 0;



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSkySphere(void)
{
	LoadModel(MODEL_SKYSPHERE, &g_SkySphere.model);
	g_SkySphere.load = TRUE;

	g_SkySphere.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_SkySphere.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_SkySphere.scl = XMFLOAT3(100.0f, 100.0f, 100.0f);

	g_SkySphere.spd  = 0.0f;			// 移動スピードクリア

	// モデルのディフューズを保存しておく。色変え対応の為。
	GetModelDiffuse(&g_SkySphere.model, &g_SkySphere.diffuse[0]);

	XMFLOAT3 pos = g_SkySphere.pos;
	

	g_SkySphere.use = TRUE;			// TRUE:生きてる

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSkySphere(void)
{
	if (g_SkySphere.load)
	{
		UnloadModel(&g_SkySphere.model);
		g_SkySphere.load = FALSE;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSkySphere(void)
{
	// スフィアを動かす
	float sphereRot = 0.0005f;
	g_SkySphere.rot.z += sphereRot;
	if (g_SkySphere.rot.z > 1.0f)
	{
		g_SkySphere.rot.z = 0.0f;
	}

#ifdef _DEBUG

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSkySphere(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);


	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_SkySphere.scl.x, g_SkySphere.scl.y, g_SkySphere.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_SkySphere.rot.x, g_SkySphere.rot.y, g_SkySphere.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_SkySphere.pos.x, g_SkySphere.pos.y, g_SkySphere.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_SkySphere.mtxWorld, mtxWorld);

	// 縁取りの設定
	SetRimLight(1);

	// モデル描画
	DrawModel(&g_SkySphere.model);

	SetRimLight(0);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
SKYSPHERE* GetSkySphere()
{
	return &g_SkySphere;
}
