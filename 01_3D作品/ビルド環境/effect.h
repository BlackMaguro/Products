//=============================================================================
//
// エフェクト処理 [effect.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			nIdxShadow;		// 影ID
	int			passedFrame;	// 経過したフレーム数
	int			existFrame;		// 継続するフレーム数（いつまで存在するか）
	BOOL		use;			// 使用しているかどうか

} EXPLOSION;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitExplosion(void);
void UninitExplosion(void);
void UpdateExplosion(void);
void DrawExplosion(void);

void ShowExplosion(const XMFLOAT3& pos,int durationTime);
void HideExplosion(void);